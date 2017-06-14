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
{
	connect(_plugin, &PackageManagerPlugin::operationCompleted,
			this, &OperationQueue::pluginOpDone);
}

void OperationQueue::setOperations(const QStringList &install, const QStringList &uninstall)
{
	auto settings = DbSettings::create();
	auto uninstallFirst = settings->value(QStringLiteral("operations/uninstall_first"), false).toBool();

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
		return; //TODO signal?
	}

	_operating = true;
	if(_nextOpFlag == Install)
		_plugin->startInstallation(_nextOp);
	else if(_nextOpFlag == Uninstall)
		_plugin->startUninstallation(_nextOp);
	else
		Q_UNREACHABLE();
	_nextOp.clear();
	_nextOpFlag = None;
}

void OperationQueue::pluginOpDone()
{
	_operating = false;
	_controller->reloadDb();
}

OperationQueue::OpertionsFlags OperationQueue::operations() const
{
	return _opFlags;
}
