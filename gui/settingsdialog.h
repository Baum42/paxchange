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
	Ui::SettingsDialog *_ui;
	QHash<QString, QWidget*> _settingsWidgets;

	explicit SettingsDialog(QWidget *parent = nullptr);
	~SettingsDialog();

	void createWidgets(QWidget *parent,
					   QFormLayout *layout,
					   bool asPlugin,
					   const QList<PackageManagerPlugin::SettingsInfo> &infos);
	QVariant value(bool asPlugin, const QString &key, const QVariant &defaultValue);
};

#endif // SETTINGSDIALOG_H
