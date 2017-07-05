#include "globalfilterwidget.h"
#include "ui_globalfilterwidget.h"

GlobalFilterWidget::GlobalFilterWidget(QWidget *parent) :
	QWidget(parent),
	_ui(new Ui::GlobalFilterWidget)
{
	_ui->setupUi(this);
}

GlobalFilterWidget::~GlobalFilterWidget()
{
	delete _ui;
}

FilterInfo::Mode GlobalFilterWidget::mode() const
{
	if(_ui->addButton->isChecked())
		return FilterInfo::Add;
	else if(_ui->skipButton->isChecked())
		return FilterInfo::Skip;
	else
		return FilterInfo::Ask;
}

void GlobalFilterWidget::setMode(FilterInfo::Mode mode)
{
	switch (mode) {
	case FilterInfo::Add:
		_ui->addButton->setChecked(true);
		break;
	case FilterInfo::Skip:
		_ui->skipButton->setChecked(true);
		break;
	case FilterInfo::Ask:
		_ui->askButton->setChecked(true);
		break;
	default:
		Q_UNREACHABLE();
		break;
	}
}

void GlobalFilterWidget::resetMode()
{
	setMode(FilterInfo::Ask);
}
