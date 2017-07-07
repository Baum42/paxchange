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
	void showUnclearDialog();

	void showMessage(const QString &text, bool critical);

	void trayMessageClicked();
	void trayAction(QSystemTrayIcon::ActivationReason reason);

	void changeDatabase();
	void mergeDatabase();
	void editPackages();
	void editFilters();
	void openSettings();

	void about();

	void operationsChanged(OperationQueue::OpertionsFlags operations);
	void showUnclear(int count);

private:
	QSystemTrayIcon *_tray;
	QMenu *_trayMenu;

	QAction *_operateAction;
	QAction *_unclearAction;
	QAction *_dialogAction;

	void enableAll(bool enable);
	void reloadIcon();
};

#endif // TRAYCONTROL_H
