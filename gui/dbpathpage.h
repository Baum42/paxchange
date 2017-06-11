#ifndef DBPATHPAGE_H
#define DBPATHPAGE_H

#include <QWizardPage>

namespace Ui {
class DbPathPage;
}

class DbPathPage : public QWizardPage
{
	Q_OBJECT

public:
	explicit DbPathPage(QWidget *parent = nullptr);
	~DbPathPage();

	void initializePage() override;
	void cleanupPage() override;

private:
	Ui::DbPathPage *ui;
};

#endif // DBPATHPAGE_H
