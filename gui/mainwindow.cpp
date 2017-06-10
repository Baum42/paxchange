#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCheckBox>

MainWindow::MainWindow(PackageManagerPlugin *plugin, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::MainWindow),
	_plugin(plugin)
{
	ui->setupUi(this);
	setupFilters();
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
	}
}

void MainWindow::reloadPackages()
{

}
