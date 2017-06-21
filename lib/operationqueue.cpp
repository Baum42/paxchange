#include "operationqueue.h"
#include "dbsettings.h"
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
	auto settings = DbSettings::create();
	auto uninstallFirst = settings->value(QStringLiteral("lib/operations/uninstall_first"), true).toBool();

	for(auto i = 0; i < 2; i++) {
		if(uninstallFirst) {
			if(!install.isEmpty()) {
				_nextOp = install;
				_nextOpFlag = Install;
				_opFlags |= _nextOpFlag;
			}
		} else {
			if(!uninstall.isEmpty()) {
				_nextOp = uninstall;
				_nextOpFlag = Uninstall;
				_opFlags |= _nextOpFlag;
			}
		}
		uninstallFirst = !uninstallFirst;
	}

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

	auto settings = DbSettings::create();
	auto useGui = settings->value(QStringLiteral("lib/operations/usegui")).toBool();

	_operating = true;
	if(_nextOpFlag == Install){
		if(useGui)
			_plugin->startGuiInstall(_nextOp);
		else
			emit startCmd(_plugin->installationCmd(_nextOp));
	}else if(_nextOpFlag == Uninstall){
		if(useGui)
			_plugin->startGuiUninstall(_nextOp);
		else
			emit startCmd(_plugin->uninstallationCmd(_nextOp));
	}else
		Q_UNREACHABLE();

	_nextOp.clear();
	_nextOpFlag = None;
}

void OperationQueue::cmdDone()
{
	_operating = false;
	_controller->reloadDb();
	_controller->sync();
}

OperationQueue::OpertionsFlags OperationQueue::operations() const
{
	return _opFlags;
}

OperationQueue::OpertionsFlag OperationQueue::nextOperation() const
{
	return _nextOpFlag;
}
