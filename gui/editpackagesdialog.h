#ifndef EDITPACKAGESDIALOG_H
#define EDITPACKAGESDIALOG_H

#include <QDialog>
#include <QCheckBox>
#include <QStringListModel>
#include <QSortFilterProxyModel>
#include "packagemanagerplugin.h"

namespace Ui {
class EditPackagesDialog;
}

class EditPackagesDialog : public QDialog
{
	Q_OBJECT

public:
	static QStringList editPackages(QWidget *parent = nullptr, const QStringList &currentPackages = QStringList(), bool *ok = nullptr);

private slots:
	void reloadPackages();

	void on_addButton_clicked();
	void on_removeButton_clicked();
	void on_clearAllButton_clicked();
	void on_regexEdit_textChanged(const QString &text);

private:
	Ui::EditPackagesDialog *_ui;
	PackageManagerPlugin *_plugin;
	QList<QCheckBox*> _boxes;

	QStringListModel *_pkgModel;
	QSortFilterProxyModel *_pkgFilter;
	QStringListModel *_dbModel;
	QSortFilterProxyModel *_dbFilter;

	explicit EditPackagesDialog(QWidget *parent = nullptr);
	~EditPackagesDialog();

	void setupFilters();
};

#endif // EDITPACKAGESDIALOG_H
