#ifndef EDITPACKAGESDIALOG_H
#define EDITPACKAGESDIALOG_H

#include <QDialog>
#include <QCheckBox>
#include <QStringListModel>
#include "packagemanagerplugin.h"

namespace Ui {
class EditPackagesDialog;
}

class EditPackagesDialog : public QDialog
{
	Q_OBJECT

public:
	static QStringList editPackages(PackageManagerPlugin *plugin, QWidget *parent = nullptr, const QStringList &currentPackages = QStringList(), bool *ok = nullptr);

private slots:
	void reloadPackages();

	void on_addButton_clicked();
	void on_removeButton_clicked();
	void on_clearAllButton_clicked();

private:
	Ui::EditPackagesDialog *_ui;
	PackageManagerPlugin *_plugin;
	QList<QCheckBox*> _boxes;

	QStringListModel *_pkgModel;
	QStringListModel *_dbModel;

	explicit EditPackagesDialog(QWidget *parent = nullptr);
	~EditPackagesDialog();

	void setupFilters();
};

#endif // EDITPACKAGESDIALOG_H
