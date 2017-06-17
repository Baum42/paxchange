#ifndef TRAYCONTROL_H
#define TRAYCONTROL_H

#include <QMenu>
#include <QObject>
#include <QSystemTrayIcon>

#include <operationqueue.h>

class TrayControl : public QObject
{
	Q_OBJECT

public:
	explicit TrayControl(QObject *parent = nullptr);
	~TrayControl();

private slots:
	void startOperation();

	void trayMessageClicked();
	void trayAction(QSystemTrayIcon::ActivationReason reason);

	void changeDatabase();
	void editPackages();

	void about();

	void operationsChanged(OperationQueue::OpertionsFlags operations);

private:
	QSystemTrayIcon *_tray;
	QMenu *_trayMenu;

	QAction *_operateAction;
};

#endif // TRAYCONTROL_H
