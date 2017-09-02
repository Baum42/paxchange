#include "unclearpackageswidget.h"
#include "ui_unclearpackageswidget.h"

#include <QPainter>
#include <QPushButton>
#include <QHeaderView>

UnclearPackagesWidget::UnclearPackagesWidget(QWidget *parent) :
	QWidget(parent),
	_ui(new Ui::UnclearPackagesWidget),
	_packageMapping(),
	_ingorePkgs()
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

UnclearHelper UnclearPackagesWidget::packages() const
{
	QList<UnclearPackageInfo> packages;
	for(auto it = _packageMapping.constBegin(); it != _packageMapping.constEnd(); ++it) {
		if(it.key() && it.key()->checkState(1) == Qt::Checked)
			packages.append(it.value());
	}
	return {packages, _ingorePkgs};
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

void UnclearPackagesWidget::setPackages(UnclearHelper packages)
{
	_ui->treeWidget->clear();

	QHash<QString, QTreeWidgetItem*> machines;
	foreach (auto package, packages.packages) {
		auto machine = machines.value(package.hostName);
		if(!machine) {
			machine = new QTreeWidgetItem(_ui->treeWidget, {package.hostName});
			machine->setFlags(machine->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsTristate);
			machine->setCheckState(1, Qt::Unchecked);
			auto ask = new QTreeWidgetItem(machine, {tr("Unclear Packages")});
			ask->setFlags(ask->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsTristate);
			ask->setCheckState(1, Qt::Unchecked);
			createBtn(ask, true);
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
		if(package.filterNames.isEmpty())
			createBtn(child, false);
		_packageMapping.insert(child, package);
	}

	_ui->treeWidget->expandAll();
}

void UnclearPackagesWidget::createBtn(QTreeWidgetItem *item, bool isAll)
{
	return;
	auto btn = new QPushButton(isAll ?
								   tr("Add Ignore Filter for all") :
								   tr("Add Ignore Filter"),
							   _ui->treeWidget->viewport());
	btn->setAutoDefault(false);
	btn->setDefault(false);
	btn->setFlat(true);

	connect(btn, &QPushButton::clicked, this, [this, item](){
		btnAction(item);
	});

	item->setData(1, ButtonWidgetRole, QVariant::fromValue(btn));
}

void UnclearPackagesWidget::btnAction(QTreeWidgetItem *item)
{
	auto info = _packageMapping.take(item);
	if(info.isValid())
		_ingorePkgs.append(info);

	for(auto i = 0; i < item->childCount(); i++) {
		auto child = item->child(i);
		btnAction(child);
	}
	auto btn = item->data(1, ButtonWidgetRole).value<QWidget*>();
	if(btn)
		btn->deleteLater();
	delete item;
}



UnclearHelper::UnclearHelper(const QList<UnclearPackageInfo> &packages, const QList<UnclearPackageInfo> &ignored) :
	packages(packages),
	ignored(ignored)
{}



UnclearDelegate::UnclearDelegate(QObject *parent) :
	QStyledItemDelegate(parent)
{}

void UnclearDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	auto widget = qobject_cast<QWidget*>(option.styleObject);
	QStyle* style = widget ? widget->style() : QApplication::style();

	auto opt = option;
	opt.features |= QStyleOptionViewItem::HasDecoration;
	opt.decorationAlignment = Qt::AlignLeft;
	opt.decorationPosition = QStyleOptionViewItem::Left;
	opt.decorationSize = {
		style->pixelMetric(QStyle::PM_IndicatorWidth, &opt, widget),
		style->pixelMetric(QStyle::PM_IndicatorHeight, &opt, widget)
	};

	QStyledItemDelegate::paint(painter, opt, index);

	QStyleOptionButton cOpt;
	if(widget)
		cOpt.initFrom(widget);
	cOpt.state = opt.state;
	cOpt.direction = opt.direction;
	cOpt.rect = opt.rect;
	cOpt.fontMetrics = opt.fontMetrics;
	cOpt.palette = opt.palette;
	cOpt.styleObject = opt.styleObject;
	cOpt.rect = opt.rect;

	cOpt.rect.setLeft(cOpt.rect.left() +
					  style->pixelMetric(QStyle::PM_IndicatorWidth, &opt, widget)/* +
					  style->pixelMetric(QStyle::PM_CheckBoxLabelSpacing, &opt, widget)*/);

	style->drawControl(QStyle::CE_CheckBox, &cOpt, painter, widget);
	painter->drawRect(opt.rect);
}
