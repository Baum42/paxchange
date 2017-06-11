#include "databasewizard.h"
#include <dialogmaster.h>

#include "dbselectionpage.h"
#include "dbpathpage.h"
#include "dbpackagespage.h"

DatabaseWizard::DatabaseWizard(QWidget *parent) :
	QWizard(parent)
{
	DialogMaster::masterDialog(this);
	setOptions(QWizard::NoBackButtonOnStartPage);
	if(wizardStyle() == QWizard::ClassicStyle)
		setWizardStyle(QWizard::ModernStyle);

	setDefaultProperty("QPathEdit", "path", SIGNAL(pathChanged(QString)));

	addPage(new DbSelectionPage(this));
	addPage(new DbPathPage(this));
	addPage(new DbPackagesPage(this));
}
