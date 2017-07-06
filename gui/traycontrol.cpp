#include "settingsdialog.h"
#include "traycontrol.h"

#include <QApplication>
#include <QDebug>
#include <dialogmaster.h>
#include <databasecontroller.h>
#include "contentdialog.h"
#include "wizard/databasewizard.h"
#include "widgets/editpackageswidget.h"
#include "widgets/filterswidget.h"
#include "widgets/globalfilterwidget.h"
#include "widgets/extrafilterswidget.h"

TrayControl::TrayControl(QObject *parent) :
	QObject(parent),
	_tray(new QSystemTrayIcon(QIcon(QStringLiteral(":/icons/tray/main.ico")), this)),
	_trayMenu(new QMenu()),
	_operateAction(nullptr),
	_dialogAction(nullptr)
{
	_operateAction = _trayMenu->addAction(QIcon(), QString(), this, &TrayControl::startOperation);
	auto font = _operateAction->font();
	font.setBold(true);
	_operateAction->setFont(font);
	_operateAction->setVisible(false);

	_trayMenu->addSeparator();
	_trayMenu->addAction(QIcon::fromTheme(QStringLiteral("package-new")),
						 tr("Change Database"),
						 this, &TrayControl::changeDatabase);
	_trayMenu->addSeparator();
	_trayMenu->addAction(QIcon::fromTheme(QStringLiteral("package-upgrade")),
						 tr("Edit Packages"),
						 this, &TrayControl::editPackages);
	_trayMenu->addAction(QIcon::fromTheme(QStringLiteral("view-filter")),
						 tr("Edit Filters"),
						 this, &TrayControl::editFilters);
	_trayMenu->addAction(QIcon::fromTheme(QStringLiteral("gtk-preferences")),
						 tr("Settings"),
						 this, &TrayControl::openSettings);
	_trayMenu->addSeparator();
	_trayMenu->addAction(QIcon::fromTheme(QStringLiteral("help-about")),
						 tr("About"),
						 this, &TrayControl::about);
	_trayMenu->addAction(QIcon::fromTheme(QStringLiteral("qt-logo")),
						 tr("About Qt"),
						 qApp, &QApplication::aboutQt);
	_trayMenu->addSeparator();
	_trayMenu->addAction(QIcon::fromTheme(QStringLiteral("gtk-quit")),
						 tr("Quit"),
						 qApp, &QApplication::quit);

	auto db = DatabaseController::instance();
	connect(db->operationQueue(), &OperationQueue::operationsChanged,
			this, &TrayControl::operationsChanged);

	connect(_tray, &QSystemTrayIcon::activated,
			this, &TrayControl::trayAction);
	connect(_tray, &QSystemTrayIcon::messageClicked,
			this, &TrayControl::trayMessageClicked);

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
	_tray->setIcon(QIcon(QStringLiteral(":/icons/tray/main.ico")));
	DatabaseController::instance()->operationQueue()->startOperation();
}

void TrayControl::trayMessageClicked()
{
	trayAction(QSystemTrayIcon::Trigger);
}

void TrayControl::trayAction(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason) {
	case QSystemTrayIcon::Trigger:
		if(_operateAction->isVisible())
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

void TrayControl::editPackages()
{
	enableAll(false);

	auto ctr = DatabaseController::instance();
	auto ok = false;
	auto packages = ContentDialog::execute<EditPackagesWidget, QStringList>(ctr->listPackages(),
																			nullptr,
																			&ok);
	if(ok)
		ctr->updateDb(packages);

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
		ctr->setGlobalMode(results[0].value<FilterInfo::Mode>());
		ctr->setFilters(results[1].value<QMap<QString,FilterInfo>>());
		ctr->setExtraFilters(results[2].value<QList<ExtraFilter>>());
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
	DialogMaster::about(nullptr,
						tr("TODO"),
						true,
						QStringLiteral("https://github.com/Baum42/pacsync"));
}

void TrayControl::operationsChanged(OperationQueue::OpertionsFlags operations)
{
	if(operations == OperationQueue::None) {
		_tray->setIcon(QIcon(QStringLiteral(":/icons/tray/main.ico")));
		_operateAction->setVisible(false);
	} else {
		auto op = DatabaseController::instance()->operationQueue();

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

		_tray->setIcon(QIcon(QStringLiteral(":/icons/tray/install.ico")));
		_tray->show();
		_tray->showMessage(tr("Packages changed!"),
						   message,
						   QSystemTrayIcon::Information);
	}
}

void TrayControl::enableAll(bool enable)
{
	foreach(auto action, _trayMenu->actions())
		action->setEnabled(enable);
	_trayMenu->actions().last()->setEnabled(true);
}
