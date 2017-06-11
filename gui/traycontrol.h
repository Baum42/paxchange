#ifndef TRAYCONTROL_H
#define TRAYCONTROL_H

#include <QMenu>
#include <QObject>
#include <QSystemTrayIcon>

class TrayControl : public QObject
{
	Q_OBJECT

public:
	explicit TrayControl(QObject *parent = nullptr);
	~TrayControl();

private slots:
	void changeDatabase();
	void editPackages();

	void about();

private:
	QSystemTrayIcon *_tray;
	QMenu *_trayMenu;
};

#endif // TRAYCONTROL_H
