#include "unclearpackageswidget.h"
#include "ui_unclearpackageswidget.h"

UnclearPackagesWidget::UnclearPackagesWidget(QWidget *parent) :
	QWidget(parent),
	_ui(new Ui::UnclearPackagesWidget),
	_packageMapping()
{
	_ui->setupUi(this);

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
		child->setFlags(child->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsTristate);
		child->setCheckState(1, Qt::Unchecked);
		_packageMapping.insert(child, package);
	}

	_ui->treeWidget->expandAll();
}
