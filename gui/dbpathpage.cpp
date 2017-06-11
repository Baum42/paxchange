#include "dbpathpage.h"
#include "ui_dbpathpage.h"

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
		setSubTitle(tr("Open an existing Pac-Sync database to be used for this machine."));
		ui->pathedit->setPathMode(QPathEdit::ExistingFile);
		setFinalPage(true);
	}
}

void DbPathPage::cleanupPage()
{
	setTitle(QString());
	setSubTitle(QString());
	ui->pathedit->clear();
}
