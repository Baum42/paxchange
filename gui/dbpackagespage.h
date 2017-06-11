#ifndef DBPACKAGESPAGE_H
#define DBPACKAGESPAGE_H

#include <QWizardPage>

class DbPackagesPage : public QWizardPage
{
	Q_OBJECT
public:
	explicit DbPackagesPage(QWidget *parent = nullptr);

	void initializePage() override;
	void cleanupPage() override;
};

#endif // DBPACKAGESPAGE_H
