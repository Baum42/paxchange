#ifndef DBMERGERDIALOG_H
#define DBMERGERDIALOG_H

#include <QDialog>
#include <databasemerger.h>

namespace Ui {
class DbMergerDialog;
}

class DbMergerDialog : public QDialog
{
	Q_OBJECT

public:
	explicit DbMergerDialog(QWidget *parent = nullptr);
	~DbMergerDialog();

	static void merge(QWidget *parent = nullptr);

public slots:
	void reject() override;

private slots:
	void on_mergeButton_clicked();

private:
	Ui::DbMergerDialog *_ui;
	DatabaseMerger *_merger;
};

#endif // DBMERGERDIALOG_H
