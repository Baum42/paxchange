#include "pacdummyplugin.h"

#include <QFile>
#include <QProcess>


PacDummyPlugin::PacDummyPlugin(QObject *parent) :
	PackageManagerPlugin(parent),
	_js(new QJsonSerializer(this))
{
	QJsonSerializer::registerListConverters<PacState>();
	QFile file(SRCDIR + QStringLiteral("/fakeman.json"));
	file.open(QIODevice::ReadOnly);

	_pacList = _js->deserializeFrom<QList<PacState>>(&file);
}


QList<PacDummyPlugin::FilterInfo> PacDummyPlugin::extraFilters()
{
	QList<PacDummyPlugin::FilterInfo> list;
	list.append({"baum", "baum is importand", true});
	list.append({"==", "==", false});
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
	QProcess process;
	process.start(qgetenv("TERM"), QStringList() << "-e" << (QStringList() << "echo install" << (noConfirm ? "--noConfirm" : "") << "&&" << qgetenv("SHELL")).join(" "));

	foreach (auto package, packages) {
		for(int i = 0; i < _pacList.size(); i++){
			if(_pacList[i].name == package)
				_pacList[i].installed = true;
		}
	}
}

void PacDummyPlugin::startUninstallation(const QStringList &packages, bool noConfirm)
{
	QProcess process;
	process.start(qgetenv("TERM"), QStringList() << "-e" << (QStringList() << "echo uninstall" << (noConfirm ? "--noConfirm" : "") << "&&" << qgetenv("SHELL")).join(" "));

	foreach (auto package, packages) {
		for(int i = 0; i < _pacList.size(); i++){
			if(_pacList[i].name == package)
				_pacList[i].installed = false;
		}
	}
}
