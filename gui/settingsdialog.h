#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

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
};

#endif // SETTINGSDIALOG_H
