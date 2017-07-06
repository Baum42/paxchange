#ifndef CONSOLEOPERATOR_H
#define CONSOLEOPERATOR_H

#include <QObject>
#include <QProcess>

class ConsoleOperator : public QObject
{
	Q_OBJECT
public:
	explicit ConsoleOperator(QObject *parent = nullptr);

private slots:
	void startCmd(QString cmd);
	void errorOccurred();

private:
	QProcess *_process;
};

#endif // CONSOLEOPERATOR_H
