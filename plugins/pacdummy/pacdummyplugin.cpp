#include "pacdummyplugin.h"

#include <QFile>


PacDummyPlugin::PacDummyPlugin(QObject *parent) :
	PackageManagerPlugin(parent),
	_js(new QJsonSerializer(this))
{
	QJsonSerializer::registerListConverters<PacState>();
	QFile file(SRCDIR + QStringLiteral("/pacdummy.json"));
	file.open(QIODevice::ReadOnly);

	_pacList = _js->deserializeFrom<QList<PacState>>(&file);
}


QList<PacDummyPlugin::FilterInfo> PacDummyPlugin::extraFilters()
{
}

QStringList PacDummyPlugin::listPackages(QList<bool> extraFilters)
{
}

void PacDummyPlugin::startInstallation(const QStringList &packages, bool noConfirm)
{
}

void PacDummyPlugin::startUninstallation(const QStringList &packages, bool noConfirm)
{
}
