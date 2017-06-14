#include "operationqueue.h"

OperationQueue::OperationQueue(QObject *parent) :
	QObject(parent),
	_nextOp(),
	_opFlags(None)
{}

void OperationQueue::addOperations(const QStringList &install, const QStringList &uninstall)
{
	if(!uninstall.isEmpty()) {
		_nextOp = uninstall;
		_opFlags |= Uninstall;
	}
	if(!install.isEmpty()) {
		_nextOp = install;
		_opFlags |= Install;
	}

	emit operationsChanged(_opFlags);
}

OperationQueue::OpertionsFlags OperationQueue::operations() const
{
	return _opFlags;
}
