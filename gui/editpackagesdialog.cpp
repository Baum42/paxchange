#include "editpackagesdialog.h"
#include "ui_editpackagesdialog.h"
#include "pluginloader.h"
#include <QDebug>
#include <dialogmaster.h>

EditPackagesDialog::EditPackagesDialog(QWidget *parent) :
	QDialog(parent),
	_ui(new Ui::EditPackagesDialog),
	_plugin(PluginLoader::plugin()),
	_boxes(),
	_pkgModel(new QStringListModel(this)),
	_pkgFilter(new QSortFilterProxyModel(this)),
	_dbModel(new QStringListModel(this)),
	_dbFilter(new QSortFilterProxyModel(this))
{
	_ui->setupUi(this);
	DialogMaster::masterDialog(this);

	_pkgFilter->setSourceModel(_pkgModel);
	_pkgFilter->setFilterCaseSensitivity(Qt::CaseInsensitive);
	_ui->localPackageListView->setModel(_pkgFilter);

	_dbFilter->setSourceModel(_dbModel);
	_dbFilter->setFilterCaseSensitivity(Qt::CaseInsensitive);
	_ui->dbPackageListView->setModel(_dbFilter);
}

EditPackagesDialog::~EditPackagesDialog()
{
	delete _ui;
}

QStringList EditPackagesDialog::editPackages(const QStringList &currentPackages, bool *ok, QWidget *parent)
{
	EditPackagesDialog dialog(parent);
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

void EditPackagesDialog::setupFilters()
{
	foreach (auto filter, _plugin->extraFilters()) {
		auto check = new QCheckBox(filter.text, _ui->groupBox);
		check->setToolTip(filter.toolTip);
		check->setChecked(filter.defaultValue);
		connect(check, &QCheckBox::clicked,
				this, &EditPackagesDialog::reloadPackages);
		_ui->checkLayout->addWidget(check);
		_boxes.append(check);
	}

	if(_boxes.isEmpty())
		_ui->groupBox->setVisible(false);
}

void EditPackagesDialog::reloadPackages()
{
	QList<bool> filters;
	foreach(auto box, _boxes)
		filters.append(box->isChecked());
	_pkgModel->setStringList(_plugin->listPackages(filters));
}

void EditPackagesDialog::on_addButton_clicked()
{
	auto indexes = _ui->localPackageListView->selectionModel()->selectedIndexes();
	auto targetList = _dbModel->stringList();
	foreach(auto filterIndex, indexes) {
		auto pkgName = _pkgModel->data(_pkgFilter->mapToSource(filterIndex), Qt::DisplayRole).toString();//TODO 5.9
		if(!targetList.contains(pkgName))
			targetList.append(pkgName);
	}
	_dbModel->setStringList(targetList);
}

void EditPackagesDialog::on_removeButton_clicked()
{
	auto indexes = _ui->dbPackageListView->selectionModel()->selectedIndexes();
	QList<QPersistentModelIndex> pIndexes;
	foreach(auto filterIndex, indexes)
		pIndexes.append(_dbFilter->mapToSource(filterIndex));
	foreach(auto index, pIndexes)
		_dbModel->removeRow(index.row(), index.parent());
}

void EditPackagesDialog::on_clearAllButton_clicked()
{
	_ui->localPackageListView->clearSelection();
	_ui->dbPackageListView->clearSelection();
}

void EditPackagesDialog::on_regexEdit_textChanged(const QString &text)
{
	QRegExp regex(text, Qt::CaseInsensitive);
	if(regex.isValid()) {
		_pkgFilter->setFilterRegExp(regex);
		_dbFilter->setFilterRegExp(regex);
	}
}
