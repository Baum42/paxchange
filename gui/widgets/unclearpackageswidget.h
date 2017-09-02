#ifndef UNCLEARPACKAGESWIDGET_H
#define UNCLEARPACKAGESWIDGET_H

#include <QStyledItemDelegate>
#include <QTreeWidget>
#include <QWidget>
#include <packagedatabase.h>

namespace Ui {
class UnclearPackagesWidget;
}

class UnclearDelegate : public QStyledItemDelegate
{
public:
	UnclearDelegate(QObject *parent = nullptr);

	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

class UnclearPackagesWidget : public QWidget
{
	Q_OBJECT

	Q_PROPERTY(QList<UnclearPackageInfo> packages READ packages WRITE setPackages USER true)

public:
	explicit UnclearPackagesWidget(QWidget *parent = nullptr);
	~UnclearPackagesWidget();

	QList<UnclearPackageInfo> packages() const;

	bool eventFilter(QObject *watched, QEvent *event) override;

public slots:
	void setPackages(QList<UnclearPackageInfo> packages);

private:
	Ui::UnclearPackagesWidget *_ui;
	QHash<QTreeWidgetItem *, UnclearPackageInfo> _packageMapping;
};

#endif // UNCLEARPACKAGESWIDGET_H
