#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
	QDialog(parent),
	_ui(new Ui::MainWindow),
	_plugin(nullptr),
	_boxes(),
	_pkgModel(new QStringListModel(this)),
	_dbModel(new QStringListModel(this))
{
	_ui->setupUi(this);
	_ui->localPackageListView->setModel(_pkgModel);
	_ui->dbPackageListView->setModel(_dbModel);
}

MainWindow::~MainWindow()
{
	delete _ui;
}

QStringList MainWindow::editPackages(PackageManagerPlugin *plugin, QWidget *parent, const QStringList &currentPackages, bool *ok)
{
	MainWindow dialog(parent);
	dialog._plugin = plugin;
	dialog._dbModel->setStringList(currentPackages);
	dialog.setupFilters();
	dialog.reloadPackages();

	if(dialog.exec() == QDialog::Accepted) {
		if(ok)
			*ok = true;
		return dialog._dbModel->stringList();
	} else {
		if(ok)
			*ok = false;
		return currentPackages;
	}
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

	if(_boxes.isEmpty())
		_ui->groupBox->setVisible(false);
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
	auto indexes = _ui->localPackageListView->selectionModel()->selectedIndexes();
	auto targetList = _dbModel->stringList();
	foreach(auto index, indexes) {
		auto pkgName = _pkgModel->data(index, Qt::DisplayRole).toString();//TODO 5.9
		if(!targetList.contains(pkgName))
			targetList.append(pkgName);
	}
	_dbModel->setStringList(targetList);
}

void MainWindow::on_removeButton_clicked()
{
	auto indexes = _ui->dbPackageListView->selectionModel()->selectedIndexes();
	QList<QPersistentModelIndex> pIndexes;
	foreach(auto index, indexes)
		pIndexes.append(index);
	foreach(auto index, pIndexes)
		_dbModel->removeRow(index.row(), index.parent());
}

void MainWindow::on_clearAllButton_clicked()
{
	_ui->localPackageListView->clearSelection();
	_ui->dbPackageListView->clearSelection();
}
