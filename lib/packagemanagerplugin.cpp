#include "packagemanagerplugin.h"


PackageManagerPlugin::PackageManagerPlugin(QObject *parent) :
	QObject(parent)
{}

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
