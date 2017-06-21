#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QFormLayout>
#include <QSettings>
#include <packagemanagerplugin.h>
namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
	Q_OBJECT

public:
	static void showSettings(QWidget *parent = nullptr);

public slots:
	void accept() override;

private:
	typedef QPair<QSettings *, QWidget *> SInfo;

	Ui::SettingsDialog *_ui;
	QHash<QString, SInfo> _settingsWidgets;
	QSettings *_localSettings;

	explicit SettingsDialog(QWidget *parent = nullptr);
	~SettingsDialog();

	void createWidgets(QWidget *parent,
					   QFormLayout *layout,
					   QSettings *settings,
					   const QList<PackageManagerPlugin::SettingsInfo> &infos);
};

#endif // SETTINGSDIALOG_H
