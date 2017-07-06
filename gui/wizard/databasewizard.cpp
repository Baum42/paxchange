#include "databasewizard.h"
#include <QDebug>
#include <QSettings>
#include <dialogmaster.h>
#include "databasecontroller.h"

#include "dbselectionpage.h"
#include "dbpathpage.h"
#include "dbwidgetpage.h"

#include "../widgets/editpackageswidget.h"
#include "../widgets/globalfilterwidget.h"
#include "../widgets/filterswidget.h"
#include "../widgets/extrafilterswidget.h"

DatabaseWizard::DatabaseWizard(QWidget *parent) :
	QWizard(parent),
	_packagePageId(-1),
	_globalModePageId(-1),
	_filterPageId(-1),
	_extraFilterPageId(-1)
{
	DialogMaster::masterDialog(this);
	setOptions(QWizard::NoBackButtonOnStartPage);
	if(wizardStyle() == QWizard::ClassicStyle)
		setWizardStyle(QWizard::ModernStyle);

	setDefaultProperty("QPathEdit", "path", SIGNAL(pathChanged(QString)));

	addPage(new DbSelectionPage(this));
	addPage(new DbPathPage(this));
	_packagePageId = addPage(new DbWidgetPage<EditPackagesWidget>(QStringLiteral("packages"), "packageList", this));
	_globalModePageId = addPage(new DbWidgetPage<GlobalFilterWidget>(QStringLiteral("globalMode"), "globalMode", this));
	_filterPageId = addPage(new DbWidgetPage<FiltersWidget>(QStringLiteral("filters"), "filters", this));
	_extraFilterPageId = addPage(new DbWidgetPage<ExtraFiltersWidget>(QStringLiteral("extraFilters"), "extraFilters", this));

	QSettings settings;
	restoreGeometry(settings.value(QStringLiteral("gui/wizard/geom")).toByteArray());
}

DatabaseWizard::~DatabaseWizard()
{
	QSettings settings;
	settings.setValue(QStringLiteral("gui/wizard/geom"), saveGeometry());
}

bool DatabaseWizard::run()
{
	DatabaseWizard w;
	return w.exec() == QDialog::Accepted;
}

void DatabaseWizard::accept()
{
	try {
		auto ctr = DatabaseController::instance();

		if(field(QStringLiteral("isCreate")).toBool()) {
			ctr->createDb(field(QStringLiteral("path")).toString(),
						  field(QStringLiteral("packages")).toStringList());
		} else if(field(QStringLiteral("isLoad")).toBool()) {
			if(hasVisitedPage(_packagePageId))
				static_cast<DbWidgetPage<EditPackagesWidget>*>(page(_packagePageId))->writeDatabase();
		}
		if(hasVisitedPage(_globalModePageId))
			static_cast<DbWidgetPage<GlobalFilterWidget>*>(page(_globalModePageId))->writeDatabase();
		if(hasVisitedPage(_filterPageId))
			static_cast<DbWidgetPage<FiltersWidget>*>(page(_filterPageId))->writeDatabase();
		if(hasVisitedPage(_extraFilterPageId))
			static_cast<DbWidgetPage<ExtraFiltersWidget>*>(page(_extraFilterPageId))->writeDatabase();

		ctr->sync();
		QWizard::accept();
	} catch(QException &e) {
		qCritical() << e.what();
		DialogMaster::critical(this, tr("Failed to create/load database!"));
		QWizard::reject();
	}
}
