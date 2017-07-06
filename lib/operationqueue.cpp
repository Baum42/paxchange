#include "operationqueue.h"
#include "pluginloader.h"
#include "databasecontroller.h"

OperationQueue::OperationQueue(DatabaseController *parent) :
	QObject(parent),
	_nextOp(),
	_nextOpFlag(None),
	_opFlags(None),
	_operating(false),
	_controller(parent),
	_plugin(PluginLoader::plugin())
{}

void OperationQueue::setOperations(const QStringList &install, const QStringList &uninstall)
{
	auto allowUninstall = _controller->readSettings(QStringLiteral("lib/operations/uninstall"), true).toBool();
	auto uninstallFirst = _controller->readSettings(QStringLiteral("lib/operations/uninstall_first"), true).toBool();

	_opFlags = None;
	for(auto i = 0; i < 2; i++) {
		if(uninstallFirst) {
			if(!install.isEmpty()) {
				_nextOp = install;
				_nextOpFlag = Install;
				_opFlags |= _nextOpFlag;
			}
		} else {
			if(allowUninstall && !uninstall.isEmpty()) {
				_nextOp = uninstall;
				_nextOpFlag = Uninstall;
				_opFlags |= _nextOpFlag;
			}
		}
		uninstallFirst = !uninstallFirst;
	}

	qDebug() << _nextOp;
	emit operationsChanged(_opFlags);
}

void OperationQueue::startOperation()
{
	if(_operating)
		return;

	if(_nextOpFlag == None) {
		emit operationsChanged(None);
		return;
	}

	auto useGui = _controller->readSettings(QStringLiteral("lib/operations/usegui"), true).toBool();

	_operating = true;
	if(_nextOpFlag == Install) {
		if(!useGui || !_plugin->startGuiInstall(_nextOp))
			emit startCmd(_plugin->installationCmd(_nextOp));
	} else if(_nextOpFlag == Uninstall) {
		if(!useGui || !_plugin->startGuiUninstall(_nextOp))
			emit startCmd(_plugin->uninstallationCmd(_nextOp));
	} else
		Q_UNREACHABLE();

	_nextOp.clear();
	_nextOpFlag = None;
}

void OperationQueue::cmdDone()
{
	try {
		if(_operating) {
			_operating = false;
			_controller->reloadDb();
		}
	} catch(QException &e) {
		qWarning() << "Failed to reload changed file:" << e.what();
		_controller->guiError(tr("Failed to reload database!"), true);
	}
}

OperationQueue::OpertionsFlags OperationQueue::operations() const
{
	return _opFlags;
}

OperationQueue::OpertionsFlag OperationQueue::nextOperation() const
{
	return _nextOpFlag;
}
