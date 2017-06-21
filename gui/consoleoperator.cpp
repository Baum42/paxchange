#include "consoleoperator.h"
#include "dbsettings.h"

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
	auto settings = DbSettings::create();
	settings->beginGroup(QStringLiteral("gui/operator"));

	QString term;
	if(settings->contains(QStringLiteral("console")))//TODO general settings
		term = settings->value(QStringLiteral("console")).toString();
	else
		term = QString::fromLatin1(qgetenv("TERM"));

	_process->start(term, {QStringLiteral("-e"), cmd});
}
