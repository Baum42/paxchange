#include "databasewizard.h"
#include <QDebug>
#include <QSettings>
#include <dialogmaster.h>
#include "databasecontroller.h"

#include "dbselectionpage.h"
#include "dbpathpage.h"
#include "dbpackagespage.h"

DatabaseWizard::DatabaseWizard(QWidget *parent) :
	QWizard(parent),
	_packagePageId(-1)
{
	DialogMaster::masterDialog(this);
	setOptions(QWizard::NoBackButtonOnStartPage);
	if(wizardStyle() == QWizard::ClassicStyle)
		setWizardStyle(QWizard::ModernStyle);

	setDefaultProperty("QPathEdit", "path", SIGNAL(pathChanged(QString)));

	addPage(new DbSelectionPage(this));
	addPage(new DbPathPage(this));
	_packagePageId = addPage(new DbPackagesPage(this));

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
				ctr->updateDb(field(QStringLiteral("packages")).toStringList());
		}
		ctr->sync();
		QWizard::accept();
	} catch(QException &e) {
		qCritical() << e.what();
		DialogMaster::critical(this, tr("Failed to create/load database!"));
		QWizard::reject();
	}
}
