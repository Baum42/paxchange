#include "filterswidget.h"
#include "ui_filterswidget.h"

#include <QFont>

#include <pluginloader.h>

FiltersWidget::FiltersWidget(QWidget *parent) :
	QWidget(parent),
	_ui(new Ui::FiltersWidget),
	_filterModel(new FilterNameModel(this)),
	_boxes()
{
	_ui->setupUi(this);

	_ui->removeButton->addAction(_ui->actionRemove_Disabled);
	connect(_ui->actionRemove_Disabled, &QAction::triggered,
			_filterModel, &FilterNameModel::removeDisabled);

	_ui->listView->setModel(_filterModel);
	connect(_ui->listView->selectionModel(), &QItemSelectionModel::currentChanged,
			this, &FiltersWidget::updateEdit);

	auto filters = PluginLoader::plugin()->extraFilters();
	foreach (auto filter, filters) {
		auto check = new QCheckBox(filter.text, _ui->extraFiltersGroup);
		check->setToolTip(filter.toolTip);
		check->setChecked(filter.defaultValue);
		_ui->checkLayout->addWidget(check);
		_boxes.append(check);
	}
	_ui->extraFiltersGroup->setVisible(!filters.isEmpty());
}

FiltersWidget::~FiltersWidget()
{
	delete _ui;
}

QMap<QString, FilterInfo> FiltersWidget::filters() const
{
	updateEdit(_ui->listView->currentIndex(), _ui->listView->currentIndex());

	QMap<QString, FilterInfo> filters;
	foreach (auto f, _filterModel->filters()) {
		const auto name = f.name;
		auto idx = 0;
		while(filters.contains(f.name))
			f.name = tr("%1-%2").arg(name).arg(++idx);
		filters.insert(f.name, f);
	}
	return filters;
}

void FiltersWidget::setFilters(QMap<QString, FilterInfo> filters)
{
	_filterModel->resetModel(filters.values());
}

void FiltersWidget::clear()
{
	_filterModel->resetModel();
}

void FiltersWidget::updateEdit(const QModelIndex &current, const QModelIndex &previous) const
{
	if(previous.isValid()) {
		FilterInfo info(_ui->nameEdit->text(), PluginLoader::currentPlugin());
		info.regex = _ui->regexEdit->text();

		info.pluginFilters.clear();
		foreach (auto box, _boxes)
			info.pluginFilters.append(box->isChecked());

		if(_ui->modeAddButton->isChecked())
			info.mode = FilterInfo::Add;
		else if(_ui->modeSkipButton->isChecked())
			info.mode = FilterInfo::Skip;
		else
			info.mode = FilterInfo::Ask;

		_filterModel->update(previous, info);
	}

	if(current.isValid()) {
		auto info = _filterModel->info(current);
		if(info.plugin == PluginLoader::currentPlugin()) {
			_ui->nameEdit->setText(info.name);
			_ui->regexEdit->setText(info.regex);

			if(info.pluginFilters.isEmpty()) {
				auto filters = PluginLoader::plugin()->extraFilters();
				for (auto i = 0; i < filters.size(); i++)
					_boxes[i]->setChecked(filters[i].defaultValue);
			} else {
				for(auto i = 0; i < _boxes.size() && i < info.pluginFilters.size(); i++)
					_boxes[i]->setChecked(info.pluginFilters[i]);
			}

			switch (info.mode) {
			case FilterInfo::Add:
				_ui->modeAddButton->setChecked(true);
				break;
			case FilterInfo::Skip:
				_ui->modeSkipButton->setChecked(true);
				break;
			case FilterInfo::Ask:
				_ui->modeAskButton->setChecked(true);
				break;
			default:
				Q_UNREACHABLE();
				break;
			}

			_ui->editWidget->setEnabled(true);
			return;
		}
	}

	_ui->editWidget->setEnabled(false);
}

void FiltersWidget::on_addButton_clicked()
{
	_filterModel->add();
	_ui->listView->setCurrentIndex(_filterModel->index(_filterModel->filters().size() - 1));
}

void FiltersWidget::on_removeButton_clicked()
{
	auto index = _ui->listView->currentIndex();
	if(index.isValid())
		_filterModel->remove(index);
}

void FiltersWidget::on_regexEdit_textChanged(const QString &text)
{
	QRegularExpression regex(text);
	auto pal = palette();
	if(!regex.isValid())
		pal.setColor(QPalette::Text, Qt::darkRed);
	_ui->regexEdit->setPalette(pal);
}




FilterNameModel::FilterNameModel(QObject *parent) :
	QAbstractListModel(parent),
	_filters()
{}

QList<FilterInfo> FilterNameModel::filters() const
{
	return _filters;
}

void FilterNameModel::resetModel(QList<FilterInfo> filters)
{
	beginResetModel();
	_filters = filters;
	endResetModel();
}

void FilterNameModel::add()
{
	beginInsertRows(QModelIndex(), _filters.size(), _filters.size());
	_filters.append(FilterInfo(tr("New Filter"), PluginLoader::currentPlugin()));
	endInsertRows();
}

void FilterNameModel::update(const QModelIndex &index, const FilterInfo &info)
{
	_filters.replace(index.row(), info);
	emit dataChanged(index, index);
}

FilterInfo FilterNameModel::info(const QModelIndex &index) const
{
	if(index.isValid())
		return _filters.value(index.row());
	else
		return FilterInfo();
}

void FilterNameModel::remove(const QModelIndex &index)
{
	if(index.isValid()) {
		beginRemoveRows(QModelIndex(), index.row(), index.row());
		_filters.removeAt(index.row());
		endRemoveRows();
	}
}

void FilterNameModel::removeDisabled()
{
	beginResetModel();
	for(auto it = _filters.begin(); it != _filters.end();) {
		if(it->plugin != PluginLoader::currentPlugin())
			it = _filters.erase(it);
		else
			it++;
	}
	endResetModel();
}

int FilterNameModel::rowCount(const QModelIndex &parent) const
{
	if(parent.isValid())
		return 0;
	else
		return _filters.size();
}

QVariant FilterNameModel::data(const QModelIndex &index, int role) const
{
	if(index.isValid() && role == Qt::DisplayRole)
		return _filters[index.row()].name;
	else
		return QVariant();
}

Qt::ItemFlags FilterNameModel::flags(const QModelIndex &index) const
{
	if(index.isValid()) {
		auto plugin = _filters[index.row()].plugin;
		Qt::ItemFlags flags = Qt::ItemNeverHasChildren;
		if(plugin == PluginLoader::currentPlugin())
			flags |= Qt::ItemIsEnabled | Qt::ItemIsSelectable;
		return flags;
	} else
		return Qt::NoItemFlags;
}
