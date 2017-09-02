#include "unclearpackageswidget.h"
#include "ui_unclearpackageswidget.h"

#include <QPainter>

UnclearPackagesWidget::UnclearPackagesWidget(QWidget *parent) :
	QWidget(parent),
	_ui(new Ui::UnclearPackagesWidget),
	_packageMapping()
{
	_ui->setupUi(this);
	_ui->treeWidget->setItemDelegateForColumn(1, new UnclearDelegate(this));
	_ui->treeWidget->setMouseTracking(true);

	_ui->treeWidget->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
}

UnclearPackagesWidget::~UnclearPackagesWidget()
{
	delete _ui;
}

QList<UnclearPackageInfo> UnclearPackagesWidget::packages() const
{
	QList<UnclearPackageInfo> packages;
	for(auto it = _packageMapping.constBegin(); it != _packageMapping.constEnd(); ++it) {
		if(it.key()->checkState(1) == Qt::Checked)
			packages.append(it.value());
	}
	return packages;
}

bool UnclearPackagesWidget::eventFilter(QObject *watched, QEvent *event)
{
	qDebug() << event->type();
	if(event->type() == QEvent::MouseButtonPress ||
	   event->type() == QEvent::MouseButtonRelease) {
		qDebug() << "repaint";
		_ui->treeWidget->repaint();
	}

	return QWidget::eventFilter(watched, event);
}

void UnclearPackagesWidget::setPackages(QList<UnclearPackageInfo> packages)
{
	_ui->treeWidget->clear();

	QHash<QString, QTreeWidgetItem*> machines;
	foreach (auto package, packages) {
		auto machine = machines.value(package.hostName);
		if(!machine) {
			machine = new QTreeWidgetItem(_ui->treeWidget, {package.hostName});
			machine->setFlags(machine->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsTristate);
			machine->setCheckState(1, Qt::Unchecked);
			auto ask = new QTreeWidgetItem(machine, {tr("Unclear Packages")});
			ask->setFlags(ask->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsTristate);
			ask->setCheckState(1, Qt::Unchecked);
			auto conf = new QTreeWidgetItem(machine, {tr("Conflicting Packages")});
			conf->setFlags(conf->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsTristate);
			conf->setCheckState(1, Qt::Unchecked);
			machines.insert(package.hostName, machine);
		}

		QTreeWidgetItem *parent = nullptr;
		if(package.filterNames.isEmpty())
			parent = machine->child(0);
		else
			parent = machine->child(1);

		auto child = new QTreeWidgetItem(parent, {package.name, package.filterNames.join(tr(", "))});
		child->setFlags(child->flags() | Qt::ItemIsUserCheckable);
		child->setCheckState(1, Qt::Unchecked);
		_packageMapping.insert(child, package);
	}

	_ui->treeWidget->expandAll();
}



UnclearDelegate::UnclearDelegate(QObject *parent) :
	QStyledItemDelegate(parent)
{}

void UnclearDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyledItemDelegate::paint(painter, option, index);
	auto opt = option;

	QStyle* style = nullptr;
	auto widget = qobject_cast<QWidget*>(option.styleObject);
	if(widget)
		style = widget->style();
	else
		style = QApplication::style();

	auto delta = style->pixelMetric(QStyle::PM_IndicatorWidth, &opt, widget) +
				 style->pixelMetric(QStyle::PM_CheckBoxLabelSpacing, &opt, widget);

	QStyleOptionButton tOpt;
	tOpt.initFrom(widget);
	tOpt.rect = opt.rect;
	tOpt.state = opt.state;
	tOpt.features = QStyleOptionButton::Flat;
	tOpt.text = tr("Add Ignore Filter");

	tOpt.rect.setLeft(tOpt.rect.x() + delta);
	tOpt.rect.setWidth(tOpt.fontMetrics.width(tOpt.text) + style->pixelMetric(QStyle::PM_ButtonMargin) * 2);
	if(tOpt.rect.contains(widget->mapFromGlobal(QCursor::pos())))
		tOpt.state |= QStyle::State_Sunken;
	qDebug() << (int)tOpt.state;

	style->drawControl(QStyle::CE_PushButton, &tOpt, painter, widget);
}

void UnclearDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	QStyledItemDelegate::setModelData(editor, model, index);
}
