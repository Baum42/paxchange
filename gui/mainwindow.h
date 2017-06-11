#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>
#include <QCheckBox>
#include <QStringListModel>
#include "packagemanagerplugin.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QDialog
{
	Q_OBJECT

public:
	static QStringList editPackages(PackageManagerPlugin *plugin, QWidget *parent = nullptr, const QStringList &currentPackages = QStringList(), bool *ok = nullptr);

signals:
	void savePackages(const QStringList &packages);

private slots:
	void reloadPackages();

	void on_addButton_clicked();
	void on_removeButton_clicked();
	void on_clearAllButton_clicked();

private:
	Ui::MainWindow *_ui;
	PackageManagerPlugin *_plugin;
	QList<QCheckBox*> _boxes;

	QStringListModel *_pkgModel;
	QStringListModel *_dbModel;

	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow();

	void setupFilters();
};

#endif // MAINWINDOW_H
