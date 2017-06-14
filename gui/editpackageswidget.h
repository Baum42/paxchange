#ifndef EDITPACKAGESDIALOG_H
#define EDITPACKAGESDIALOG_H

#include <QDialog>
#include <QCheckBox>
#include <QStringListModel>
#include <QSortFilterProxyModel>
#include <packagemanagerplugin.h>
#include "packagemodel.h"

namespace Ui {
class EditPackagesWidget;
}

class EditPackagesWidget : public QWidget
{
	Q_OBJECT

	Q_PROPERTY(QStringList packages READ packages WRITE setPackages USER true)

public:
	explicit EditPackagesWidget(QWidget *parent = nullptr);
	~EditPackagesWidget();

	QStringList packages() const;

public slots:
	void setPackages(QStringList packages);
	void clear();

private slots:
	void reloadPackages();

	void on_addButton_clicked();
	void on_removeButton_clicked();
	void on_clearAllButton_clicked();
	void on_regexEdit_textChanged(const QString &text);

private:
	Ui::EditPackagesWidget *_ui;
	PackageManagerPlugin *_plugin;
	QList<QCheckBox*> _boxes;

	QStringListModel *_pkgModel;
	QSortFilterProxyModel *_pkgFilter;
	PackageModel *_dbModel;
	QSortFilterProxyModel *_dbFilter;

	void setupFilters();
};

#endif // EDITPACKAGESDIALOG_H
