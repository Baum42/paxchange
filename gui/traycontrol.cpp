#include "traycontrol.h"

#include <QApplication>
#include <QDebug>
#include <dialogmaster.h>

#include "editpackagesdialog.h"

TrayControl::TrayControl(QObject *parent) :
	QObject(parent),
	_tray(new QSystemTrayIcon(QApplication::windowIcon(), this)),
	_trayMenu(new QMenu())
{
	_trayMenu->addAction(QIcon::fromTheme("package-upgrade"), tr("Edit Packages"), this, &TrayControl::debugMainWindow);
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

void TrayControl::debugMainWindow()
{
	qDebug() << EditPackagesDialog::editPackages();
}

void TrayControl::about()
{
	DialogMaster::about(nullptr,
						tr("TODO"),
						true,
						QStringLiteral("https://github.com/Baum42/pacsync"));
}
