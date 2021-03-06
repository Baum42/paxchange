#include "packagemanagerplugin.h"
#include <utility>

PackageManagerPlugin::PackageManagerPlugin(QObject *parent) :
	QObject(parent)
{}

void PackageManagerPlugin::initialize() {}

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

QSettings *PackageManagerPlugin::createLocalSettings(QObject *parent) const
{
	auto settings = new QSettings(parent);
	settings->beginGroup(QStringLiteral("plugins/%1")
						 .arg(QString::fromUtf8(metaObject()->className())));
	return settings;
}

void PackageManagerPlugin::settingsChanged() {}

void PackageManagerPlugin::forwardedArguments(QStringList args)
{
	Q_UNUSED(args);
}

PackageManagerPlugin::SettingsInfo::SettingsInfo(QString displayName, QString description, QString settingsKeys, int type, QVariant defaultValue, QByteArray widgetClassName, QVariantMap widgetProperties) :
	displayName(std::move(displayName)),
	description(std::move(description)),
	settingsKeys(std::move(settingsKeys)),
	type(type),
	defaultValue(std::move(defaultValue)),
	widgetClassName(std::move(widgetClassName)),
	widgetProperties(std::move(widgetProperties))
{}
