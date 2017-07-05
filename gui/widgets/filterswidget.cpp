#include "filterswidget.h"
#include "ui_filterswidget.h"

FiltersWidget::FiltersWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::FiltersWidget)
{
	ui->setupUi(this);
}

FiltersWidget::~FiltersWidget()
{
	delete ui;
}

QMap<QString, FilterInfo> FiltersWidget::filters() const
{
	return _filters;
}

void FiltersWidget::setFilters(QMap<QString, FilterInfo> filters)
{
	_filters = filters;
}

void FiltersWidget::clear()
{
	_filters.clear();
}
