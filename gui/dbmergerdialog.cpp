#include "dbmergerdialog.h"
#include "ui_dbmergerdialog.h"
#include <dialogmaster.h>

DbMergerDialog::DbMergerDialog(QWidget *parent) :
	QDialog(parent),
	_ui(new Ui::DbMergerDialog),
	_merger(new DatabaseMerger(this))
{
	_ui->setupUi(this);
	_ui->deleteCheckBox->setVisible(false);
	DialogMaster::masterDialog(this);
}

DbMergerDialog::~DbMergerDialog()
{
	delete _ui;
}

void DbMergerDialog::merge(QWidget *parent)
{
	DbMergerDialog dialog(parent);
	dialog.exec();
}

void DbMergerDialog::accept()
{
	if(_ui->deleteCheckBox->isChecked())
		QFile::remove(_ui->pathedit->path());
	QDialog::accept();
}

void DbMergerDialog::on_mergeButton_clicked()
{
	if(!QFile::exists(_ui->pathedit->path()))
		return;

	_ui->pathedit->setEnabled(false);
	_ui->mergeButton->setEnabled(false);

	if(_merger->mergeDb(_ui->pathedit->path())) {
		_ui->buttonBox->setStandardButtons(QDialogButtonBox::Close);
		_ui->deleteCheckBox->setVisible(true);

		auto log = _merger->log();
		if(log.isEmpty())
			_ui->textBrowser->append(tr("Merged without any conflicts!"));
		else {
			for(auto entry : log)
				_ui->textBrowser->append(entry);
		}

		DialogMaster::information(this, tr("Merge completed!"));
		accept();
	} else {
		DialogMaster::critical(this,
							   tr("Failed to merge the specified database with the current one! "
								  "The file does either not exist or is corrupted."),
							   tr("Merge failed!"));
		reject();
	}
}
