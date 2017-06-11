#ifndef DBPACKAGESPAGE_H
#define DBPACKAGESPAGE_H

#include <QWizardPage>
#include "editpackageswidget.h"

class DbPackagesPage : public QWizardPage
{
	Q_OBJECT

public:
	explicit DbPackagesPage(QWidget *parent = nullptr);

	void initializePage() override;
	void cleanupPage() override;

private:
	EditPackagesWidget *_widget;
};

#endif // DBPACKAGESPAGE_H
