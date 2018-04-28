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

	Q_PROPERTY(FilterInfo::Mode mode READ mode WRITE setMode RESET resetMode USER true)

public:
	explicit GlobalFilterWidget(QWidget *parent = nullptr);
	~GlobalFilterWidget() override;

	FilterInfo::Mode mode() const;

public slots:
	void setMode(FilterInfo::Mode mode);
	void resetMode();

private:
	Ui::GlobalFilterWidget *_ui;
};

#endif // GLOBALFILTERWIDGET_H
