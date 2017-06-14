#ifndef DATABASEWIZARD_H
#define DATABASEWIZARD_H

#include <QWizard>

class DatabaseWizard : public QWizard
{
	Q_OBJECT
public:
	explicit DatabaseWizard(QWidget *parent = nullptr);
	~DatabaseWizard();

	static bool run();

public slots:
	void accept() override;

private:
	int _packagePageId;
};

#endif // DATABASEWIZARD_H
