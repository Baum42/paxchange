#include "unclearpackageswidget.h"
#include "ui_unclearpackageswidget.h"

#include <QPainter>
#include <QPushButton>
#include <QHeaderView>
#include <QMouseEvent>

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

UnclearHelper UnclearPackagesWidget::packages() const
{
	UnclearHelper packages;
	for(auto it = _packageMapping.constBegin(); it != _packageMapping.constEnd(); ++it) {
		if(it.key()->checkState(1) == Qt::Checked)
			packages.sync.append(it.value());
		else if(it.key()->data(1, IgnoreCheckRole).toInt() == Qt::Checked)
			packages.ignore.append(it.value());
	}
	return packages;
}

void UnclearPackagesWidget::setPackages(UnclearHelper packages)
{
	_ui->treeWidget->clear();

	QHash<QString, QTreeWidgetItem*> machines;
	foreach (auto package, packages.sync) {
		auto machine = machines.value(package.hostName);
		if(!machine) {
			machine = new UnclearItem(_ui->treeWidget, {package.hostName});
			machine->setFlags(machine->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsTristate);
			machine->setCheckState(1, Qt::Unchecked);
			auto ask = new UnclearItem(machine, {tr("Unclear Packages")}, true);
			ask->setFlags(ask->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsTristate);
			ask->setCheckState(1, Qt::Unchecked);
			auto conf = new UnclearItem(machine, {tr("Conflicting Packages")});
			conf->setFlags(conf->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsTristate);
			conf->setCheckState(1, Qt::Unchecked);
			machines.insert(package.hostName, machine);
		}

		UnclearItem *parent = nullptr;
		if(package.filterNames.isEmpty())
			parent = static_cast<UnclearItem*>(machine->child(0));
		else
			parent = static_cast<UnclearItem*>(machine->child(1));

		auto child = new UnclearItem(parent, {package.name, package.filterNames.join(tr(", "))}, parent->isIgnorable());
		child->setFlags(child->flags() | Qt::ItemIsUserCheckable);
		child->setCheckState(1, Qt::Unchecked);
		_packageMapping.insert(child, package);
	}

	_ui->treeWidget->expandAll();
}



UnclearHelper::UnclearHelper(const QList<UnclearPackageInfo> &packages, const QList<UnclearPackageInfo> &ignored) :
	sync(packages),
	ignore(ignored)
{}


UnclearDelegate::UnclearDelegate(QObject *parent) :
	QStyledItemDelegate(parent)
{}

void UnclearDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	auto opt = option;
	initStyleOption(&opt, index);

	auto widget = qobject_cast<QWidget*>(option.styleObject);
	auto style = widget ? widget->style() : QApplication::style();

	style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);

	//fake a second checkbox
	if(index.data(UnclearPackagesWidget::IsIgnorableRole).toBool()) {
		painter->save();
		painter->setClipRect(opt.rect);
		initIgnOpt(style, widget, &opt);
		style->drawPrimitive(QStyle::PE_IndicatorItemViewItemCheck, &opt, painter, widget);
		painter->restore();
	}
}

bool UnclearDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
	Q_ASSERT(event);
	Q_ASSERT(model);

	if(index.data(UnclearPackagesWidget::IsIgnorableRole).toBool()) {
		auto value = index.data(UnclearPackagesWidget::IgnoreCheckRole);

		if ((event->type() == QEvent::MouseButtonRelease)
			|| (event->type() == QEvent::MouseButtonDblClick)
			|| (event->type() == QEvent::MouseButtonPress)) {
			auto opt = option;
			initStyleOption(&opt, index);
			auto widget = qobject_cast<QWidget*>(option.styleObject);
			auto style = widget ? widget->style() : QApplication::style();
			initIgnOpt(style, widget, &opt);

			auto mEvent = static_cast<QMouseEvent*>(event);
			if (mEvent->button() == Qt::LeftButton && opt.rect.contains(mEvent->pos())) {
				if ((event->type() == QEvent::MouseButtonPress)
					|| (event->type() == QEvent::MouseButtonDblClick))
					return true;

				auto state = static_cast<Qt::CheckState>(value.toInt());
				state = (state == Qt::Checked) ? Qt::Unchecked : Qt::Checked;
				return model->setData(index, state, UnclearPackagesWidget::IgnoreCheckRole);
			}
		}
	}

	return QStyledItemDelegate::editorEvent(event, model, option, index);
}

void UnclearDelegate::initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const
{
	auto widget = qobject_cast<QWidget*>(option->styleObject);
	QStyle* style = widget ? widget->style() : QApplication::style();

	QStyledItemDelegate::initStyleOption(option, index);

	if(index.data(UnclearPackagesWidget::IsIgnorableRole).toBool()) {
		option->features |= QStyleOptionViewItem::HasDecoration;
		option->decorationAlignment = Qt::AlignLeft;
		option->decorationPosition = QStyleOptionViewItem::Left;
		option->decorationSize = {
			style->pixelMetric(QStyle::PM_IndicatorWidth, option, widget),
			style->pixelMetric(QStyle::PM_IndicatorHeight, option, widget)
		};
	}
}

void UnclearDelegate::initIgnOpt(QStyle *style, QWidget *widget, QStyleOptionViewItem *option) const
{
	option->rect = style->proxy()->subElementRect(QStyle::SE_ItemViewItemCheckIndicator, option, widget);
	option->rect.moveLeft(option->rect.left() +
					  style->pixelMetric(QStyle::PM_IndicatorWidth, option, widget) +
					  style->pixelMetric(QStyle::PM_CheckBoxLabelSpacing, option, widget));

	option->state = option->state & ~QStyle::State_HasFocus;
	switch (option->index.data(UnclearPackagesWidget::IgnoreCheckRole).toInt()) {
	case Qt::Unchecked:
		option->state |= QStyle::State_Off;
		break;
	case Qt::PartiallyChecked:
		option->state |= QStyle::State_NoChange;
		break;
	case Qt::Checked:
		option->state |= QStyle::State_On;
		break;
	}
}



UnclearItem::UnclearItem(QTreeWidgetItem *parent, const QStringList &strings, bool isIgnorable) :
	QTreeWidgetItem(parent, strings)
{
	setData(1, UnclearPackagesWidget::IsIgnorableRole, isIgnorable);
}

UnclearItem::UnclearItem(QTreeWidget *view, const QStringList &strings) :
	QTreeWidgetItem(view, strings)
{
	setData(1, UnclearPackagesWidget::IsIgnorableRole, false);
}

bool UnclearItem::isIgnorable() const
{
	return data(1, UnclearPackagesWidget::IsIgnorableRole).toBool();
}

QVariant UnclearItem::data(int column, int role) const
{
	if(role == UnclearPackagesWidget::IgnoreCheckRole)
		return QTreeWidgetItem::data(column + 2, Qt::CheckStateRole);
	else
		return QTreeWidgetItem::data(column, role);
}

void UnclearItem::setData(int column, int role, const QVariant &value)
{
	if(role == UnclearPackagesWidget::IgnoreCheckRole) {
		role = Qt::CheckStateRole;
		QTreeWidgetItem::setData(column + 2, role, value);
		if(value.toInt() == Qt::Checked)
			QTreeWidgetItem::setData(column, role, Qt::Unchecked);
	} else if(role == Qt::CheckStateRole) {
		QTreeWidgetItem::setData(column, role, value);
		if(value.toInt() == Qt::Checked)
			QTreeWidgetItem::setData(column + 2, role, Qt::Unchecked);
	} else
		QTreeWidgetItem::setData(column, role, value);
}
