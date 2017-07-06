#ifndef TRAYCONTROL_H
#define TRAYCONTROL_H

#include <QMenu>
#include <QObject>
#include <QSystemTrayIcon>
#include <packagedatabase.h>

#include <operationqueue.h>

class TrayControl : public QObject
{
	Q_OBJECT

public:
	explicit TrayControl(QObject *parent = nullptr);
	~TrayControl();

	void show();

private slots:
	void startOperation();

	void trayMessageClicked();
	void trayAction(QSystemTrayIcon::ActivationReason reason);

	void changeDatabase();
	void editPackages();
	void editFilters();
	void openSettings();

	void about();

	void operationsChanged(OperationQueue::OpertionsFlags operations);
	void showUnclear(const QList<UnclearPackageInfo> &unclearPkg);

private:
	QSystemTrayIcon *_tray;
	QMenu *_trayMenu;

	QAction *_operateAction;
	QAction *_dialogAction;

	void enableAll(bool enable);
};

#endif // TRAYCONTROL_H
