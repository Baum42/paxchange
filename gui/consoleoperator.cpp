#include "consoleoperator.h"

#include <QTemporaryFile>
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
	connect(_process, &QProcess::errorOccurred,
			this, &ConsoleOperator::errorOccurred);
}

void ConsoleOperator::startCmd(QString cmd)
{
	auto term = DatabaseController::instance()->readSettings(QStringLiteral("gui/operator/console")).toString();
	if(term.isEmpty())
		term = QString::fromLatin1(qgetenv("TERM"));

	auto tmp = new QTemporaryFile(this);
	tmp->setAutoRemove(false);
	tmp->open();

	QTextStream stream(tmp);
	stream << QStringLiteral("#!/bin/sh\n")
		   << cmd << QStringLiteral("\necho -e\nread -n 1 -p \"")
		   << tr("Press any key to continue...")
		   << QStringLiteral("\"\n");
	stream.flush();
	tmp->close();
	tmp->setPermissions(tmp->permissions() | QFileDevice::ExeUser);

	auto fileName = tmp->fileName();
	delete tmp;

	connect(_process, QOverload<int>::of(&QProcess::finished),
			this, [fileName](){
		QFile f(fileName);
		f.remove();
	});

	_process->start(term, {QStringLiteral("-e"), fileName});
}

void ConsoleOperator::errorOccurred()
{
	auto db = DatabaseController::instance();
	db->operationQueue()->cmdDone();
	qCritical() << "Console command failed" << _process->errorString();
	db->guiError(tr("Failed to start console for un/install operations"), true);
}
