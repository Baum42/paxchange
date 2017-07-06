#ifndef UNCLEARPACKAGESWIDGET_H
#define UNCLEARPACKAGESWIDGET_H

#include <QTreeWidget>
#include <QWidget>
#include <packagedatabase.h>

namespace Ui {
class UnclearPackagesWidget;
}

class UnclearPackagesWidget : public QWidget
{
	Q_OBJECT

	Q_PROPERTY(QList<UnclearPackageInfo> packages READ packages WRITE setPackages USER true)

public:
	explicit UnclearPackagesWidget(QWidget *parent = nullptr);
	~UnclearPackagesWidget();

	QList<UnclearPackageInfo> packages() const;

public slots:
	void setPackages(QList<UnclearPackageInfo> packages);

private:
	Ui::UnclearPackagesWidget *_ui;
	QHash<QTreeWidgetItem *, UnclearPackageInfo> _packageMapping;
};

#endif // UNCLEARPACKAGESWIDGET_H
