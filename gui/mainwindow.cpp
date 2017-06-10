#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(PackageManagerPlugin *plugin, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::MainWindow),
	_plugin(plugin)
{
	ui->setupUi(this);
	setupFilters();
	reloadPackages();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::setupFilters()
{
	foreach (auto filter, _plugin->extraFilters()) {
		auto check = new QCheckBox(filter.text, ui->groupBox);
		check->setToolTip(filter.toolTip);
		check->setChecked(filter.defaultValue);
		connect(check, &QCheckBox::clicked,
				this, &MainWindow::reloadPackages);
		ui->checkLayout->addWidget(check);
		_boxes.append(check);
	}
}

void MainWindow::reloadPackages()
{
	QList<bool> filters;
	foreach(auto box, _boxes)
		filters.append(box->isChecked());
	qDebug() << _plugin->listPackages(filters);
}
