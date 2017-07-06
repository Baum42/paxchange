#ifndef FILTERSWIDGET_H
#define FILTERSWIDGET_H

#include <QWidget>
#include <QAbstractListModel>
#include <packagedatabase.h>
#include <QCheckBox>

namespace Ui {
class FiltersWidget;
}

class FilterNameModel : public QAbstractListModel
{
public:
	FilterNameModel(QObject *parent = nullptr);

	QList<FilterInfo> filters() const;
	void resetModel(QList<FilterInfo> filters = {});

	void add();
	void update(const QModelIndex &index, const FilterInfo &info);
	FilterInfo info(const QModelIndex &index) const;
	void remove(const QModelIndex &index);
	void removeDisabled();

	int rowCount(const QModelIndex &parent) const override;
	QVariant data(const QModelIndex &index, int role) const override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
	QList<FilterInfo> _filters;
};

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

private slots:
	void updateEdit(const QModelIndex &current, const QModelIndex &previous) const;

	void on_action_Add_triggered();
	void on_action_Remove_triggered();
	void on_regexEdit_textChanged(const QString &text);

private:
	Ui::FiltersWidget *_ui;
	FilterNameModel *_filterModel;
	QList<QCheckBox*> _boxes;

	void reloadTitles(const QString &current = QString());
};

#endif // FILTERSWIDGET_H
