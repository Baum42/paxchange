#ifndef DBSELECTIONPAGE_H
#define DBSELECTIONPAGE_H

#include <QWizardPage>

namespace Ui {
class DbSelectionPage;
}

class DbSelectionPage : public QWizardPage
{
	Q_OBJECT

public:
	explicit DbSelectionPage(QWidget *parent = nullptr);
	~DbSelectionPage() override;

	void cleanupPage() override;

private:
	Ui::DbSelectionPage *ui;
};

#endif // DBSELECTIONPAGE_H
