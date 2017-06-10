#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(PackageManagerPlugin *plugin, QWidget *parent) :
	QDialog(parent),
	_ui(new Ui::MainWindow),
	_plugin(plugin),
	_boxes(),
	_pkgModel(new QStringListModel(this))
{
	_ui->setupUi(this);
	_ui->localPackageListView->setModel(_pkgModel);

	setupFilters();
	reloadPackages();
}

MainWindow::~MainWindow()
{
	delete _ui;
}

void MainWindow::setupFilters()
{
	foreach (auto filter, _plugin->extraFilters()) {
		auto check = new QCheckBox(filter.text, _ui->groupBox);
		check->setToolTip(filter.toolTip);
		check->setChecked(filter.defaultValue);
		connect(check, &QCheckBox::clicked,
				this, &MainWindow::reloadPackages);
		_ui->checkLayout->addWidget(check);
		_boxes.append(check);
	}
}

void MainWindow::reloadPackages()
{
	QList<bool> filters;
	foreach(auto box, _boxes)
		filters.append(box->isChecked());
	_pkgModel->setStringList(_plugin->listPackages(filters));
}

void MainWindow::on_addButton_clicked()
{

}

void MainWindow::on_removeButton_clicked()
{

}

void MainWindow::on_clearAllButton_clicked()
{
	_ui->localPackageListView->clearSelection();
	_ui->dbPackageListView->clearSelection();
}
