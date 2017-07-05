#include "dbpackagespage.h"
#include <QVBoxLayout>
#include "databasecontroller.h"

DbPackagesPage::DbPackagesPage(QWidget *parent) :
	DbWidgetPage(QStringLiteral("packages"), parent)
{}

void DbPackagesPage::initializePage()
{
	DbWidgetPage::initializePage();
	if(field(QStringLiteral("isLoad")).toBool())
		_widget->setPackages(DatabaseController::instance()->listPackages());
	 else
		_widget->setPackages(QStringList());
}
