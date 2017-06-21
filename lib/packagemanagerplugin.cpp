#include "packagemanagerplugin.h"
#include "dbsettings.h"

PackageManagerPlugin::PackageManagerPlugin(QObject *parent) :
	QObject(parent)
{}

QSettings *PackageManagerPlugin::createPluginSettings(QObject *parent) const
{
	auto settings = DbSettings::create(parent);
	settings->beginGroup(QStringLiteral("plugins/%1")
						 .arg(metaObject()->className()));
	return settings;
}

void PackageManagerPlugin::settingsChanged() {}
