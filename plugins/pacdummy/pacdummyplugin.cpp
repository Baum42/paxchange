#include "pacdummyplugin.h"

#include <QFile>
#include <QProcess>


PacDummyPlugin::PacDummyPlugin(QObject *parent) :
	PackageManagerPlugin(parent),
	_js(new QJsonSerializer(this))
{
	QJsonSerializer::registerListConverters<PacState>();
	/*QFile file(SRCDIR + QStringLiteral("/pacdummy.json"));
	file.open(QIODevice::ReadOnly);

	_pacList = _js->deserializeFrom<QList<PacState>>(&file);*/
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
	/*QProcess process;
	process.start("gedit", QStringList() << docPath);*/
}

void PacDummyPlugin::startUninstallation(const QStringList &packages, bool noConfirm)
{
}
