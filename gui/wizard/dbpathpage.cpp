#include "dbpathpage.h"
#include "ui_dbpathpage.h"
#include <QAbstractButton>
#include <QDebug>
#include <dialogmaster.h>
#include <databasecontroller.h>

DbPathPage::DbPathPage(QWidget *parent) :
	QWizardPage(parent),
	ui(new Ui::DbPathPage)
{
	ui->setupUi(this);

	registerField(QStringLiteral("path*"), ui->pathedit);
}

DbPathPage::~DbPathPage()
{
	delete ui;
}

void DbPathPage::initializePage()
{
	if(field(QStringLiteral("isCreate")).toBool()) {
		setTitle(tr("Create new Database"));
		setSubTitle(tr("Create a new Database file to store your package synchronization state to."));
		ui->pathedit->setPathMode(QPathEdit::AnyFile);
		setFinalPage(false);
	} else if(field(QStringLiteral("isLoad")).toBool()) {
		setTitle(tr("Open Existing Database"));
		setSubTitle(tr("Open an existing Paxchange database to be used for this machine."));
		ui->pathedit->setPathMode(QPathEdit::ExistingFile);
		ui->pathedit->setPath(DatabaseController::instance()->currentPath());
		setFinalPage(true);
	}
}

void DbPathPage::cleanupPage()
{
	setTitle(QString());
	setSubTitle(QString());
	ui->pathedit->clear();
}

bool DbPathPage::validatePage()
{
	if(field(QStringLiteral("isLoad")).toBool()) {
		try {
			DatabaseController::instance()->loadDb(ui->pathedit->path());
			return true;
		} catch(QException &e) {
			qWarning() << e.what();
			DialogMaster::warning(this, tr("Failed to load database!"));
			return false;
		}
	}

	return true;
}
