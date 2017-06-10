#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>
#include "packagemanagerplugin.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QDialog
{
	Q_OBJECT

public:
	explicit MainWindow(PackageManagerPlugin *plugin, QWidget *parent = nullptr);
	~MainWindow();

public slots:
	void reloadPackages();

private:
	Ui::MainWindow *ui;
	PackageManagerPlugin *_plugin;

	void setupFilters();
};

#endif // MAINWINDOW_H
