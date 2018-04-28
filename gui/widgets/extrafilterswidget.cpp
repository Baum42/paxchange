#include "extrafilterswidget.h"
#include "ui_extrafilterswidget.h"

#include <QComboBox>

ExtraFiltersWidget::ExtraFiltersWidget(QWidget *parent) :
	QWidget(parent),
	_ui(new Ui::ExtraFiltersWidget)
{
	_ui->setupUi(this);

	_ui->addButton->setDefaultAction(_ui->action_Add_Filter);
	_ui->removeButton->setDefaultAction(_ui->action_Remove_Filter);
	_ui->treeWidget->addActions({
									_ui->action_Add_Filter,
									_ui->action_Remove_Filter
								});

	_ui->treeWidget->header()->setSectionResizeMode(0, QHeaderView::Stretch);
	_ui->treeWidget->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
}

ExtraFiltersWidget::~ExtraFiltersWidget()
{
	delete _ui;
}

QList<ExtraFilter> ExtraFiltersWidget::extraFilters() const
{
	QList<ExtraFilter> filters;
	for(auto i = 0; i < _ui->treeWidget->topLevelItemCount(); i++) {
		ExtraFilter filter;
		auto item = _ui->treeWidget->topLevelItem(i);
		filter.regex = item->text(0);
		filter.mode = item->data(1, Qt::UserRole).value<FilterInfo::Mode>();
		filters.append(filter);
	}
	return filters;
}

void ExtraFiltersWidget::setExtraFilters(QList<ExtraFilter> extraFilters)
{
	_ui->treeWidget->clear();
	for(auto filter : extraFilters)
		addFilter(filter);
}

void ExtraFiltersWidget::on_action_Add_Filter_triggered()
{
	addFilter({QStringLiteral(".*"), FilterInfo::Ask});
}

void ExtraFiltersWidget::on_action_Remove_Filter_triggered()
{
	auto item = _ui->treeWidget->currentItem();
	if(item)
		delete item;
}

void ExtraFiltersWidget::on_treeWidget_itemChanged(QTreeWidgetItem *item, int column)
{
	if(item && column == 0) {
		QRegularExpression regex(item->text(0));
		if(!regex.isValid())
			item->setForeground(0, Qt::red);
		else
			item->setForeground(0, item->foreground(1));
	}
}

void ExtraFiltersWidget::addFilter(const ExtraFilter &filter)
{
	auto item = new QTreeWidgetItem(_ui->treeWidget);
	item->setText(0, filter.regex);
	item->setToolTip(0, filter.regex);
	item->setData(1, Qt::UserRole, QVariant::fromValue(filter.mode));
	item->setToolTip(1, modeName(filter.mode));

	item->setFlags(item->flags() | Qt::ItemIsEditable);

	auto box = new QComboBox(_ui->treeWidget);
	box->addItem(modeName(FilterInfo::Add), FilterInfo::Add);
	box->addItem(modeName(FilterInfo::Skip), FilterInfo::Skip);
	box->addItem(modeName(FilterInfo::Ask), FilterInfo::Ask);
	box->setCurrentIndex(filter.mode);
	connect(box, QOverload<int>::of(&QComboBox::currentIndexChanged),
			this, [item](int index){
		item->setData(1, Qt::UserRole, QVariant::fromValue(static_cast<FilterInfo::Mode>(index)));
	});
	_ui->treeWidget->setItemWidget(item, 1, box);
}


QString ExtraFiltersWidget::modeName(FilterInfo::Mode mode)
{
	switch(mode) {
	case FilterInfo::Add:
		return tr("Synchronize");
	case FilterInfo::Skip:
		return tr("Don't Synchronize");
	case FilterInfo::Ask:
		return tr("Ask");
	default:
		Q_UNREACHABLE();
		return QString();
	}
}
