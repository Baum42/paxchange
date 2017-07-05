#ifndef FILTERSWIDGET_H
#define FILTERSWIDGET_H

#include <QWidget>
#include <packagedatabase.h>

namespace Ui {
class FiltersWidget;
}

class FiltersWidget : public QWidget
{
	Q_OBJECT

	Q_PROPERTY(QMap<QString, FilterInfo> filters READ filters WRITE setFilters RESET clear USER true)

public:
	explicit FiltersWidget(QWidget *parent = nullptr);
	~FiltersWidget();

	QMap<QString, FilterInfo> filters() const;

public slots:
	void setFilters(QMap<QString, FilterInfo> filters);
	void clear();

private:
	Ui::FiltersWidget *ui;
	QMap<QString, FilterInfo> _filters;
};

#endif // FILTERSWIDGET_H
