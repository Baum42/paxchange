#include "dbpackagespage.h"
#include <QVBoxLayout>
#include "databasecontroller.h"

DbPackagesPage::DbPackagesPage(QWidget *parent) :
	QWizardPage(parent),
	_widget(new EditPackagesWidget(this))
{
	setLayout(new QVBoxLayout(this));
	layout()->addWidget(_widget);

	setTitle(_widget->windowTitle());
	setSubTitle(tr("Select the packages you want to add to the database. "
				   "Those packages will be synchronized across your machines."));

	registerField(QStringLiteral("packages"), _widget, "packages");
}

void DbPackagesPage::initializePage()
{
	if(field(QStringLiteral("isLoad")).toBool())
		_widget->setPackages(DatabaseController::instance()->listPackages());
	else
		_widget->setPackages(QStringList());
}

void DbPackagesPage::cleanupPage()
{
	_widget->clear();
}
