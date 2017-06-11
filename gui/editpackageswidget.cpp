#include "editpackageswidget.h"
#include "ui_editpackageswidget.h"
#include "pluginloader.h"
#include <QDebug>
#include <dialogmaster.h>

EditPackagesWidget::EditPackagesWidget(QWidget *parent) :
	QWidget(parent),
	_ui(new Ui::EditPackagesWidget),
	_plugin(PluginLoader::plugin()),
	_boxes(),
	_pkgModel(new QStringListModel(this)),
	_pkgFilter(new QSortFilterProxyModel(this)),
	_dbModel(new QStringListModel(this)),
	_dbFilter(new QSortFilterProxyModel(this))
{
	_ui->setupUi(this);

	_pkgFilter->setSourceModel(_pkgModel);
	_pkgFilter->setFilterCaseSensitivity(Qt::CaseInsensitive);
	_ui->localPackageListView->setModel(_pkgFilter);

	_dbFilter->setSourceModel(_dbModel);
	_dbFilter->setFilterCaseSensitivity(Qt::CaseInsensitive);
	_ui->dbPackageListView->setModel(_dbFilter);

	setupFilters();
	reloadPackages();
}

EditPackagesWidget::~EditPackagesWidget()
{
	delete _ui;
}

QStringList EditPackagesWidget::packages() const
{
	return _dbModel->stringList();
}

void EditPackagesWidget::setPackages(QStringList packages)
{
	_dbModel->setStringList(packages);
}

void EditPackagesWidget::clear()
{
	_dbModel->setStringList(QStringList());
	auto filters = _plugin->extraFilters();
	for(auto i = 0; i < _boxes.size(); i++)
		_boxes[i]->setChecked(filters[i].defaultValue);
	_ui->regexEdit->clear();
	_ui->clearAllButton->click();
}

void EditPackagesWidget::setupFilters()
{
	foreach (auto filter, _plugin->extraFilters()) {
		auto check = new QCheckBox(filter.text, _ui->groupBox);
		check->setToolTip(filter.toolTip);
		check->setChecked(filter.defaultValue);
		connect(check, &QCheckBox::clicked,
				this, &EditPackagesWidget::reloadPackages);
		_ui->checkLayout->addWidget(check);
		_boxes.append(check);
	}

	if(_boxes.isEmpty())
		_ui->groupBox->setVisible(false);
}

void EditPackagesWidget::reloadPackages()
{
	QList<bool> filters;
	foreach(auto box, _boxes)
		filters.append(box->isChecked());
	_pkgModel->setStringList(_plugin->listPackages(filters));
}

void EditPackagesWidget::on_addButton_clicked()
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

void EditPackagesWidget::on_removeButton_clicked()
{
	auto indexes = _ui->dbPackageListView->selectionModel()->selectedIndexes();
	QList<QPersistentModelIndex> pIndexes;
	foreach(auto filterIndex, indexes)
		pIndexes.append(_dbFilter->mapToSource(filterIndex));
	foreach(auto index, pIndexes)
		_dbModel->removeRow(index.row(), index.parent());
}

void EditPackagesWidget::on_clearAllButton_clicked()
{
	_ui->localPackageListView->clearSelection();
	_ui->dbPackageListView->clearSelection();
}

void EditPackagesWidget::on_regexEdit_textChanged(const QString &text)
{
	QRegExp regex(text, Qt::CaseInsensitive);
	if(regex.isValid()) {
		_pkgFilter->setFilterRegExp(regex);
		_dbFilter->setFilterRegExp(regex);
	}
}
