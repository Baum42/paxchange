#include "comboboxconfig.h"
#include "databasecontroller.h"
#include "pluginloader.h"
#include "syncedsettings.h"

SyncedSettings::SyncedSettings(){}

SyncedSettings::~SyncedSettings()
{
	if(!_changes.isEmpty())
		DatabaseController::instance()->writeSettings(_changes);
}

QVariant SyncedSettings::value(const QString &key, const QVariant &overwriteDefault) const
{
	auto rKey = realKey(key);

	if(_changes.contains(rKey))
		return _changes.value(rKey);
	else {
		auto value = DatabaseController::instance()->readSettings(key);
		if(value.isValid())
			return value;
		else if(overwriteDefault.isValid())
			return overwriteDefault;
		else
			return defaultValue(key);
	}
}

void SyncedSettings::setValue(const QString &key, const QVariant &value)
{
	_changes.insert(realKey(key), value);
}

void SyncedSettings::remove(const QString &key)
{
	_changes.insert(realKey(key), QVariant());
}

void SyncedSettings::beginGroup(const QString &key)
{
	_groups.append(key);
}

void SyncedSettings::endGroup()
{
	if(!_groups.isEmpty())
		_groups.removeLast();
}

QString SyncedSettings::pluginKey(const QString &key)
{
	QStringList rGroups({
							QStringLiteral("plugin"),
							PluginLoader::currentPlugin(),
							key
						});
	return rGroups.join(QLatin1Char('/'));
}

QString SyncedSettings::realKey(const QString &key) const
{
	QStringList rGroups({
							QStringLiteral("plugin"),
							PluginLoader::currentPlugin()
						});
	rGroups.append(_groups);
	rGroups.append(key);
	return rGroups.join(QLatin1Char('/'));
}

QVariant SyncedSettings::defaultValue(const QString &localKey) const
{
	auto rGroups = _groups;
	rGroups.append(localKey);
	auto key = rGroups.join(QLatin1Char('/'));

	foreach(auto info, PluginLoader::plugin()->listSettings()) {
		if(info.settingsKeys == key){
			if(qMetaTypeId<ComboboxConfig>() == info.defaultValue.userType())
				return info.defaultValue.value<ComboboxConfig>().defaultValue;
			else
				return info.defaultValue;
		}
	}

	return QVariant();
}
