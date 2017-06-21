#include "packagemanagerplugin.h"
#include "dbsettings.h"

PackageManagerPlugin::PackageManagerPlugin(QObject *parent) :
	QObject(parent)
{}

bool PackageManagerPlugin::startGuiInstall(const QStringList &packages)
{
	Q_UNUSED(packages);
	return false;
}

bool PackageManagerPlugin::startGuiUninstall(const QStringList &packages)
{
	Q_UNUSED(packages);
	return false;
}

QSettings *PackageManagerPlugin::createPluginSettings(QObject *parent) const
{
	auto settings = DbSettings::create(parent);
	settings->beginGroup(QStringLiteral("plugins/%1")
						 .arg(QString::fromUtf8(metaObject()->className())));
	return settings;
}

void PackageManagerPlugin::settingsChanged() {}

PackageManagerPlugin::SettingsInfo::SettingsInfo(QString displayName, QString description, QString settingsKeys, int type, QVariant defaultValue, QByteArray widgetClassName) :
	displayName(displayName),
	description(description),
	settingsKeys(settingsKeys),
	type(type),
	defaultValue(defaultValue),
	widgetClassName(widgetClassName)
{

}
