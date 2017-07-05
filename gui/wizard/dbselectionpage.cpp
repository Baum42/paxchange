#include "dbselectionpage.h"
#include "ui_dbselectionpage.h"

DbSelectionPage::DbSelectionPage(QWidget *parent) :
	QWizardPage(parent),
	ui(new Ui::DbSelectionPage)
{
	ui->setupUi(this);

	registerField(QStringLiteral("isCreate"), ui->createButton);
	registerField(QStringLiteral("isLoad"), ui->loadButton);
}

DbSelectionPage::~DbSelectionPage()
{
	delete ui;
}

void DbSelectionPage::cleanupPage()
{
	ui->createButton->setChecked(true);
}
