#include "pacdummyplugin.h"

#include <QFile>


PacDummyPlugin::PacDummyPlugin(QObject *parent) :
	PackageManagerPlugin(parent),
	_js(new QJsonSerializer(this)),
	_process(new QProcess(this))
{
	QJsonSerializer::registerListConverters<PacState>();
	QFile file(SRCDIR + QStringLiteral("/fakeman.json"));
	file.open(QIODevice::ReadOnly);

	_pacList = _js->deserializeFrom<QList<PacState>>(&file);
}


QList<PacDummyPlugin::FilterInfo> PacDummyPlugin::extraFilters()
{
	QList<PacDummyPlugin::FilterInfo> list;
	list.append({"&baum", "baum is importand", true});
	list.append({"==", QString(), false});
	list.append({"42", "meaning of life", true});
	return list;
}

QStringList PacDummyPlugin::listPackages(QList<bool> extraFilters)
{
	QStringList list;
	foreach (auto pacState, _pacList) {
		if(!pacState.installed)
			continue;

		if(!extraFilters[0] || (extraFilters[0] && pacState.baumFilter))
			list.append(pacState.name);
	}
	return list;
}

void PacDummyPlugin::startInstallation(const QStringList &packages, bool noConfirm)
{
	Q_UNUSED(noConfirm);

	_process->start(qgetenv("TERM"), QStringList() << "-e" << "sleep 5");

	foreach (auto package, packages) {
		for(int i = 0; i < _pacList.size(); i++){
			if(_pacList[i].name == package)
				_pacList[i].installed = true;
		}
	}

	emit operationCompleted();
}

void PacDummyPlugin::startUninstallation(const QStringList &packages, bool noConfirm)
{
	Q_UNUSED(noConfirm);

	_process->start(qgetenv("TERM"), QStringList() << "-e" << "sleep 5");

	foreach (auto package, packages) {
		for(int i = 0; i < _pacList.size(); i++){
			if(_pacList[i].name == package)
				_pacList[i].installed = false;
		}
	}

	emit operationCompleted();
}
