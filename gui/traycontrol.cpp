#include "dbmergerdialog.h"
#include "settingsdialog.h"
#include "traycontrol.h"

#include <QApplication>
#include <dialogmaster.h>
#include <databasecontroller.h>
#include <QTimer>
#include "contentdialog.h"
#include "wizard/databasewizard.h"
#include "widgets/editpackageswidget.h"
#include "widgets/filterswidget.h"
#include "widgets/globalfilterwidget.h"
#include "widgets/extrafilterswidget.h"
#include "widgets/unclearpackageswidget.h"

TrayControl::TrayControl(QObject *parent) :
	QObject(parent),
	_tray(new QSystemTrayIcon(QIcon(QStringLiteral(":/icons/tray/main.ico")), this)),
	_trayMenu(new QMenu()),
	_operateAction(nullptr),
	_unclearAction(nullptr),
	_dialogAction(nullptr)
{
	qRegisterMetaType<QSystemTrayIcon::ActivationReason>("QSystemTrayIcon::ActivationReason");
	auto db = DatabaseController::instance();

	_operateAction = _trayMenu->addAction(QIcon(), QString(), this, &TrayControl::startOperation);
	auto font = _operateAction->font();
	font.setBold(true);
	_operateAction->setFont(font);
	_operateAction->setVisible(false);

	_unclearAction = _trayMenu->addAction(QIcon::fromTheme(QStringLiteral("package-available-locked")),
										  tr("Review unclear packages"),
										  this, &TrayControl::showUnclearDialog);
	_unclearAction->setFont(font);
	_unclearAction->setVisible(false);

	_trayMenu->addSeparator();
	_trayMenu->addAction(QIcon::fromTheme(QStringLiteral("package-new")),
						 tr("Change Database"),
						 this, &TrayControl::changeDatabase);

	auto dbMenu = _trayMenu->addMenu(QIcon::fromTheme(QStringLiteral("package-available")),
									 tr("Database Actions"));
	dbMenu->addAction(QIcon::fromTheme(QStringLiteral("merge")),
					  tr("Merge Database"),
					  this, &TrayControl::mergeDatabase);
	dbMenu->addAction(QIcon::fromTheme(QStringLiteral("view-refresh")),
					  tr("Synchronize"),
					  db, &DatabaseController::sync);
	dbMenu->addSeparator();
	dbMenu->addAction(QIcon::fromTheme(QStringLiteral("package-upgrade")),
					  tr("Edit Packages"),
					  this, &TrayControl::editPackages);
	dbMenu->addAction(QIcon::fromTheme(QStringLiteral("view-filter")),
					  tr("Edit Filters"),
					  this, &TrayControl::editFilters);
	dbMenu->addSeparator();
	dbMenu->addAction(QIcon::fromTheme(QStringLiteral("gtk-preferences")),
					  tr("Settings"),
					  this, &TrayControl::openSettings);

	_trayMenu->addSeparator();
	_trayMenu->addAction(QIcon::fromTheme(QStringLiteral("help-about")),
						 tr("About"),
						 this, &TrayControl::about);
	_trayMenu->addAction(QIcon::fromTheme(QStringLiteral("gtk-quit")),
						 tr("Quit"),
						 qApp, &QApplication::quit);

	connect(db->operationQueue(), &OperationQueue::operationsChanged,
			this, &TrayControl::operationsChanged);
	connect(db, &DatabaseController::unclearPackagesChanged,
			this, &TrayControl::showUnclear);
	connect(db, &DatabaseController::guiError,
			this, &TrayControl::showMessage,
			Qt::QueuedConnection);

	connect(_tray, &QSystemTrayIcon::activated,
			this, &TrayControl::trayAction,
			Qt::QueuedConnection);
	connect(_tray, &QSystemTrayIcon::messageClicked,
			this, &TrayControl::trayMessageClicked,
			Qt::QueuedConnection);

	_tray->setContextMenu(_trayMenu);
	_tray->setToolTip(QApplication::applicationDisplayName());
}

TrayControl::~TrayControl()
{
	_trayMenu->deleteLater();
}

void TrayControl::show()
{
	_tray->show();
}

void TrayControl::startOperation()
{
	_operateAction->setVisible(false);
	reloadIcon();
	DatabaseController::instance()->operationQueue()->startOperation();
}

void TrayControl::showUnclearDialog()
{
	_unclearAction->setVisible(false);
	reloadIcon();

	enableAll(false);

	auto ctr = DatabaseController::instance();
	auto ok = false;
	auto packages = ContentDialog::execute<UnclearPackagesWidget, QList<UnclearPackageInfo>>(ctr->listUnclearPackages(),
																							 nullptr,
																							 &ok);
	if(ok)
		ctr->clearPackages(packages);

	enableAll(true);
}

void TrayControl::showMessage(const QString &text, bool critical)
{
	QTimer::singleShot(500, this, [=](){
		_tray->setIcon(QIcon(QStringLiteral(":/icons/tray/error.ico")));
		_tray->show();
		_tray->showMessage(critical ? tr("Error") : tr("Warning"),
						   text,
						   critical ? QSystemTrayIcon::Critical : QSystemTrayIcon::Warning);
	});
}

void TrayControl::trayMessageClicked()
{
	trayAction(QSystemTrayIcon::Trigger);
}

void TrayControl::trayAction(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason) {
	case QSystemTrayIcon::Trigger:
		if(_unclearAction->isVisible())
			_unclearAction->trigger();
		else if(_operateAction->isVisible())
			_operateAction->trigger();
		break;
	default:
		break;
	}
}

void TrayControl::changeDatabase()
{
	enableAll(false);
	DatabaseWizard::run();
	enableAll(true);
}

void TrayControl::mergeDatabase()
{
	DbMergerDialog::merge();
}

void TrayControl::editPackages()
{
	enableAll(false);

	auto ctr = DatabaseController::instance();
	auto ok = false;
	auto packages = ContentDialog::execute<EditPackagesWidget, QStringList>(ctr->listPackages(),
																			nullptr,
																			&ok);
	if(ok) {
		ctr->updateDb(packages);
		ctr->sync();
	}

	enableAll(true);
}

void TrayControl::editFilters()
{
	enableAll(false);

	auto ctr = DatabaseController::instance();
	auto results = ContentDialog::execute(tr("Edit Filters"),
										  QList<QWidget*>({
											  new GlobalFilterWidget(),
											  new FiltersWidget(),
											  new ExtraFiltersWidget()
										  }),
										  {
											  QVariant::fromValue(ctr->globalMode()),
											  QVariant::fromValue(ctr->filters()),
											  QVariant::fromValue(ctr->extraFilters())
										  },
										  1);
	if(!results.isEmpty()) {
		ctr->beginSaveTransaction();
		ctr->setGlobalMode(results[0].value<FilterInfo::Mode>());
		ctr->setFilters(results[1].value<QMap<QString,FilterInfo>>());
		ctr->setExtraFilters(results[2].value<QList<ExtraFilter>>());
		ctr->commitSave();
	}

	enableAll(true);
}

void TrayControl::openSettings()
{
	enableAll(false);
	SettingsDialog::showSettings();
	enableAll(true);
}

void TrayControl::about()
{
	static int counter = 0;
	if(++counter == 0b00101010)
		QApplication::setWindowIcon(QIcon(QStringLiteral(":/icons/28106788.png")));

	auto info = DialogMaster::createInformation();
	info.icon = QApplication::windowIcon();
	info.windowTitle = tr("About");
	info.title = tr("%1 — Version %2")
				 .arg(QApplication::applicationDisplayName())
				 .arg(QApplication::applicationVersion());
	info.text = tr("<p>A tool to synchronize installed packages across multiple machines.</p>"
				   "<p>Loaded Plugin: <i>%1</i><br/>"
				   "Qt-Version: <a href=\"https://www.qt.io/\">%2</a></p>"
				   "<p>Developed by: Felix Barz and Mike Zeller<br/>"
				   "Project Website: <a href=\"https://github.com/Baum42/paxchange\">https://github.com/Baum42/paxchange</a><br/>"
				   "License: <a href=\"https://github.com/Baum42/paxchange/blob/master/LICENSE\">BSD 3 Clause</a></p>"
				   "<p>Icons based on <a href=\"http://www.flaticon.com/authors/roundicons\">Roundicons</a> from <a href=\"http://www.flaticon.com\">www.flaticon.com</a> and licensed by <a href=\"http://creativecommons.org/licenses/by/3.0/\">CC 3.0 BY</a></p>")
				.arg(PluginLoader::currentPlugin())
				.arg(QStringLiteral(QT_VERSION_STR));

	info.buttons = QMessageBox::Close | QMessageBox::Help;
	info.buttonTexts.insert(QMessageBox::Help, tr("About Qt"));
	info.defaultButton = QMessageBox::Close;
	info.escapeButton = QMessageBox::Close;

	auto btn = DialogMaster::messageBox(info);
	if(btn == QMessageBox::Help)
		QApplication::aboutQt();
}

void TrayControl::operationsChanged(OperationQueue::OpertionsFlags operations)
{
	if(operations == OperationQueue::None) {
		_operateAction->setVisible(false);
		reloadIcon();
	} else {
		auto op = DatabaseController::instance()->operationQueue();
		if(op->isOperating())
			return;

		auto message = tr("The package database has changed. Packages have to be %1.");
		if(op->nextOperation() == OperationQueue::Install) {
			message = message.arg(tr("installed"));
			_operateAction->setIcon(QIcon::fromTheme(QStringLiteral("package-install")));
			_operateAction->setText(tr("Install new packages"));
		} else if(op->nextOperation() == OperationQueue::Uninstall) {
			message = message.arg(tr("uninstalled"));
			_operateAction->setIcon(QIcon::fromTheme(QStringLiteral("package-remove")));
			_operateAction->setText(tr("Uninstall old packages"));
		}
		_operateAction->setVisible(true);

		reloadIcon();
		_tray->show();
		_tray->showMessage(tr("Packages changed!"),
						   message,
						   QSystemTrayIcon::Information);
	}
}

void TrayControl::showUnclear(int count)
{
	if(count == 0) {
		_unclearAction->setVisible(false);
		reloadIcon();
	} else {
		_unclearAction->setVisible(true);
		reloadIcon();
		_tray->show();
		_tray->showMessage(tr("Packages unclear!"),
						   tr("There are %L1 packages that need to be revised for synchronization.")
						   .arg(count),
						   QSystemTrayIcon::Information);
	}
}

void TrayControl::enableAll(bool enable)
{
	foreach(auto action, _trayMenu->actions())
		action->setEnabled(enable);
	_trayMenu->actions().last()->setEnabled(true);
}

void TrayControl::reloadIcon()
{
	if(_unclearAction->isVisible())
		_tray->setIcon(QIcon(QStringLiteral(":/icons/tray/question.ico")));
	else if(_operateAction->isVisible())
		_tray->setIcon(QIcon(QStringLiteral(":/icons/tray/install.ico")));
	else
		_tray->setIcon(QIcon(QStringLiteral(":/icons/tray/main.ico")));
}
