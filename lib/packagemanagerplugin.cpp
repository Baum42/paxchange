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

PackageManagerPlugin::SettingsInfo::SettingsInfo(QString displayName, QString description, QString settingsKeys, int type, QVariant defaultValue, QString widgetClassName) :
	displayName(displayName),
	description(description),
	settingsKeys(settingsKeys),
	type(type),
	defaultValue(defaultValue),
	widgetClassName(widgetClassName)
{

}
