#ifndef UNCLEARPACKAGESWIDGET_H
#define UNCLEARPACKAGESWIDGET_H

#include <QStyledItemDelegate>
#include <QTreeWidget>
#include <QWidget>
#include <packagedatabase.h>

namespace Ui {
class UnclearPackagesWidget;
}

class UnclearItem : public QTreeWidgetItem
{
public:
	explicit UnclearItem(QTreeWidgetItem *parent, const QStringList &strings, bool isIgnorable = false);
	explicit UnclearItem(QTreeWidget *view, const QStringList &strings);

	bool isIgnorable() const;

	QVariant data(int column, int role) const override;
	void setData(int column, int role, const QVariant &value) override;
};

class UnclearDelegate : public QStyledItemDelegate
{
public:
	UnclearDelegate(QObject *parent = nullptr);

	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
	bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

protected:
	void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const override;

private:
	void initIgnOpt(QStyle *style, QWidget *widget, QStyleOptionViewItem *option) const;
};

class UnclearHelper
{
public:
	UnclearHelper(QList<UnclearPackageInfo> sync = {},
				  QList<UnclearPackageInfo> ignore = {});

	QList<UnclearPackageInfo> sync;
	QList<UnclearPackageInfo> ignore;
};

class UnclearPackagesWidget : public QWidget
{
	Q_OBJECT

	Q_PROPERTY(UnclearHelper packages READ packages WRITE setPackages USER true)

public:
	enum ExtraRoles {
		IsIgnorableRole = Qt::UserRole + 1,
		IgnoreCheckRole
	};
	Q_ENUM(ExtraRoles)

	explicit UnclearPackagesWidget(QWidget *parent = nullptr);
	~UnclearPackagesWidget() override;

	UnclearHelper packages() const;

public slots:
	void setPackages(UnclearHelper packages);

private:
	Ui::UnclearPackagesWidget *_ui;
	QHash<QTreeWidgetItem *, UnclearPackageInfo> _packageMapping;
};

Q_DECLARE_METATYPE(UnclearHelper)

#endif // UNCLEARPACKAGESWIDGET_H
