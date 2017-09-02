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

class UnclearHelper
{
public:
	UnclearHelper(const QList<UnclearPackageInfo> &packages = {},
				  const QList<UnclearPackageInfo> &ignored = {});

	QList<UnclearPackageInfo> packages;
	QList<UnclearPackageInfo> ignored;
};

class UnclearPackagesWidget : public QWidget
{
	Q_OBJECT

	Q_PROPERTY(UnclearHelper packages READ packages WRITE setPackages USER true)

public:
	enum ExtraRoles {
		ButtonWidgetRole = Qt::UserRole + 1,
	};
	Q_ENUM(ExtraRoles)

	explicit UnclearPackagesWidget(QWidget *parent = nullptr);
	~UnclearPackagesWidget();

	UnclearHelper packages() const;

	bool eventFilter(QObject *watched, QEvent *event) override;

public slots:
	void setPackages(UnclearHelper packages);

private:
	Ui::UnclearPackagesWidget *_ui;
	QHash<QTreeWidgetItem *, UnclearPackageInfo> _packageMapping;
	QList<UnclearPackageInfo> _ingorePkgs;

	void createBtn(QTreeWidgetItem *item, bool isAll);
	void btnAction(QTreeWidgetItem *item);
};

Q_DECLARE_METATYPE(UnclearHelper)

#endif // UNCLEARPACKAGESWIDGET_H
