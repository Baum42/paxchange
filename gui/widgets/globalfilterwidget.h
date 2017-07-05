#ifndef GLOBALFILTERWIDGET_H
#define GLOBALFILTERWIDGET_H

#include <QWidget>
#include <packagedatabase.h>

namespace Ui {
class GlobalFilterWidget;
}

class GlobalFilterWidget : public QWidget
{
	Q_OBJECT

	Q_PROPERTY(FilterInfo::Mode mode READ mode WRITE setMode USER true)

public:
	explicit GlobalFilterWidget(QWidget *parent = 0);
	~GlobalFilterWidget();

	FilterInfo::Mode mode() const;

public slots:
	void setMode(FilterInfo::Mode mode);

private:
	Ui::GlobalFilterWidget *_ui;
};

#endif // GLOBALFILTERWIDGET_H
