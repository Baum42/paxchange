#include "pacdummyplugin.h"

#include <QCoreApplication>


PacDummyPlugin::PacDummyPlugin(QObject *parent) :
	PackageManagerPlugin(parent),
	_js(new QJsonSerializer(this)),
	_process(new QProcess(this)),
	_file(new QFile(QCoreApplication::applicationDirPath() + QStringLiteral("/fakeman.json"), this))
{
	QJsonSerializer::registerListConverters<PacState>();

	if(QFile::copy(QStringLiteral(":/fakeman.json"), _file->fileName()))
		_file->setPermissions(QFile::ReadOwner | QFile::WriteOwner);

	_file->open(QIODevice::ReadOnly);
	_pacList = _js->deserializeFrom<QList<PacState>>(_file);
	_file->close();
}


QList<PacDummyPlugin::FilterInfo> PacDummyPlugin::extraFilters()
{
	QList<PacDummyPlugin::FilterInfo> list;
	list.append({"&baum", "baum is importand", true});
	list.append({"42", QString(), false});
	return list;
}

QStringList PacDummyPlugin::listAllPackages()
{
	QStringList list;
	foreach (auto pacState, _pacList) {
		if(!pacState.installed)
			continue;
		list.append(pacState.name);
	}

	return list;
}

QStringList PacDummyPlugin::listPackages(QList<bool> extraFilters)
{
	QStringList list;
	foreach (auto pacState, _pacList) {
		if(!pacState.installed)
			continue;

		if(extraFilters[0] && !pacState.filterBaum)
			continue;

		if(extraFilters[1] && !pacState.filter42)
			continue;

		list.append(pacState.name);
	}
	return list;
}

void PacDummyPlugin::startInstallation(const QStringList &packages)
{
	_process->start(qgetenv("TERM"), QStringList() << "-e" << "sleep 5");

	bool stateChanged = false;
	foreach (auto package, packages) {
		for(int i = 0; i < _pacList.size(); i++){
			if(_pacList[i].name == package){
				_pacList[i].installed = true;
				stateChanged = true;
			}
		}
	}

	if(stateChanged){
		_file->open(QIODevice::WriteOnly);
		_js->serializeTo<QList<PacState>>(_file, _pacList);
		_file->close();
	}

	emit operationCompleted();
}

void PacDummyPlugin::startUninstallation(const QStringList &packages)
{
	_process->start(qgetenv("TERM"), QStringList() << "-e" << "sleep 5");

	bool stateChanged = false;
	foreach (auto package, packages) {
		for(int i = 0; i < _pacList.size(); i++){
			if(_pacList[i].name == package){
				_pacList[i].installed = false;
				stateChanged = true;
			}
		}
	}

	if(stateChanged){
		_file->open(QIODevice::WriteOnly);
		_js->serializeTo<QList<PacState>>(_file, _pacList);
		_file->close();
	}

	emit operationCompleted();
}
