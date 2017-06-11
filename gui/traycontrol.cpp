#include "databasewizard.h"
#include "traycontrol.h"

#include <QApplication>
#include <QDebug>
#include <dialogmaster.h>
#include <databasecontroller.h>
#include "contentdialog.h"
#include "editpackageswidget.h"

TrayControl::TrayControl(QObject *parent) :
	QObject(parent),
	_tray(new QSystemTrayIcon(QIcon(QStringLiteral(":/icons/tray.ico")), this)),
	_trayMenu(new QMenu())
{
	_trayMenu->addAction(QIcon::fromTheme("package-new"), tr("Change Database"), this, &TrayControl::changeDatabase);
	_trayMenu->addSeparator();
	_trayMenu->addAction(QIcon::fromTheme("package-upgrade"), tr("Edit Packages"), this, &TrayControl::editPackages);
	_trayMenu->addSeparator();
	_trayMenu->addAction(QIcon::fromTheme("help-about"), tr("About"), this, &TrayControl::about);
	_trayMenu->addAction(QIcon::fromTheme("qt-logo"), tr("About Qt"), qApp, &QApplication::aboutQt);
	_trayMenu->addSeparator();
	_trayMenu->addAction(QIcon::fromTheme("gtk-quit"), tr("Quit"), qApp, &QApplication::quit);

	_tray->setContextMenu(_trayMenu);
	_tray->setToolTip(QApplication::applicationDisplayName());
	_tray->show();
}

TrayControl::~TrayControl()
{
	_trayMenu->deleteLater();
}

void TrayControl::changeDatabase()
{
	DatabaseWizard::run();
}

void TrayControl::editPackages()
{
	auto ctr = DatabaseController::instance();
	auto ok = false;
	auto packages = ContentDialog::execute<EditPackagesWidget, QStringList>(ctr->listPackages(),
																			nullptr,
																			&ok);
	if(ok) {
		ctr->updateDb(packages);
		ctr->sync();
	}
}

void TrayControl::about()
{
	DialogMaster::about(nullptr,
						tr("TODO"),
						true,
						QStringLiteral("https://github.com/Baum42/pacsync"));
}
