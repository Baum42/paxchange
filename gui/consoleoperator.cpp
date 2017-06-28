#include "consoleoperator.h"

#include <databasecontroller.h>

ConsoleOperator::ConsoleOperator(QObject *parent) :
	QObject(parent),
	_process(new QProcess(this))
{
	auto db = DatabaseController::instance();

	connect(db->operationQueue(), &OperationQueue::startCmd,
			this, &ConsoleOperator::startCmd);
	connect(_process, QOverload<int>::of(&QProcess::finished),
			db->operationQueue(), &OperationQueue::cmdDone);
}

void ConsoleOperator::startCmd(QString cmd)
{
	auto term = DatabaseController::instance()->readSettings(QStringLiteral("gui/operator/console")).toString();
	if(term.isEmpty())
		term = QString::fromLatin1(qgetenv("TERM"));

	_process->start(term, {QStringLiteral("-e"), cmd});
}
