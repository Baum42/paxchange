#ifndef EXTRAFILTERSWIDGET_H
#define EXTRAFILTERSWIDGET_H

#include <QTreeWidget>
#include <QWidget>
#include <packagedatabase.h>

namespace Ui {
class ExtraFiltersWidget;
}

class ExtraFiltersWidget : public QWidget
{
	Q_OBJECT

	Q_PROPERTY(QList<ExtraFilter> extraFilters READ extraFilters WRITE setExtraFilters USER true)

public:
	explicit ExtraFiltersWidget(QWidget *parent = nullptr);
	~ExtraFiltersWidget() override;

	QList<ExtraFilter> extraFilters() const;

public slots:
	void setExtraFilters(QList<ExtraFilter> extraFilters);

private slots:
	void on_action_Add_Filter_triggered();
	void on_action_Remove_Filter_triggered();
	void on_treeWidget_itemChanged(QTreeWidgetItem *item, int column);

private:
	Ui::ExtraFiltersWidget *_ui;

	void addFilter(const ExtraFilter &filter);

	static QString modeName(FilterInfo::Mode mode);
};

#endif // EXTRAFILTERSWIDGET_H
