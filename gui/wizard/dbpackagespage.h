#ifndef DBPACKAGESPAGE_H
#define DBPACKAGESPAGE_H

#include <QWizardPage>
#include "dbwidgetpage.h"
#include "../widgets/editpackageswidget.h"

class DbPackagesPage : public DbWidgetPage<EditPackagesWidget>
{
	Q_OBJECT

public:
	explicit DbPackagesPage(QWidget *parent = nullptr);

	void initializePage() override;
};

#endif // DBPACKAGESPAGE_H
