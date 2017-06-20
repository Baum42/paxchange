#include "pluginloader.h"
#include <QDir>
#include <QJsonArray>
#include <QJsonValue>
#include <QDebug>
#include <QGlobalStatic>
#ifndef QT_NO_DEBUG
#include <QCoreApplication>
#else
#include <QLibraryInfo>
#endif

Q_GLOBAL_STATIC(PluginLoader, pluginLoader)

PluginLoader::PluginLoader(QObject *parent) :
	QObject(parent),
	_availablePlugins(),
	_plugin(nullptr)
{
#ifndef QT_NO_DEBUG
	auto path = QCoreApplication::applicationDirPath() + QStringLiteral("/..");
#else
	auto path = QLibraryInfo::location(QLibraryInfo::PluginsPath);
#endif
	path += QStringLiteral("/pacsync");

	QDir plugDir(path);
	plugDir.setFilter(QDir::Files | QDir::Readable);
	foreach (auto plg, plugDir.entryList()) {
		auto loader = new QPluginLoader(plugDir.absoluteFilePath(plg), this);
		auto meta = loader->metaData();
		if(meta[QStringLiteral("IID")].toString() == QStringLiteral(PackageManagerPlugin_iid)) {
			auto keys = meta[QStringLiteral("MetaData")].toObject()[QStringLiteral("Keys")].toArray();
			if(!keys.isEmpty()) {
				foreach (auto key, keys) {
					qDebug() << "Found plugin for key" << key.toString();
					_availablePlugins.insert(key.toString(), loader);
				}
				continue;
			}
		}

		loader->deleteLater();
	}
}

QStringList PluginLoader::availablePlugins()
{
	return pluginLoader->_availablePlugins.keys();
}

void PluginLoader::loadPlugin(const QString &overwrite)
{
	auto name = overwrite;
	if(name.isEmpty())
		name = pluginLoader->defaultPlugin();

	auto loader = pluginLoader->_availablePlugins.value(name, nullptr);
	if(!loader)
		throw PluginLoadException(QStringLiteral("The given key does not name a plugin"));

	if(loader->load()) {
		auto object = loader->instance();
		pluginLoader->_plugin = qobject_cast<PackageManagerPlugin*>(object);
		if(!pluginLoader->_plugin)
			throw PluginLoadException(QStringLiteral("The loaded plugin is not a PackageManagerPlugin"));
	} else
		throw PluginLoadException(QStringLiteral("Failed to load plugin with error : %1")
								  .arg(loader->errorString()));
}

PackageManagerPlugin *PluginLoader::plugin()
{
	return pluginLoader->_plugin;
}

QString PluginLoader::defaultPlugin() const
{
	if(_availablePlugins.contains(QStringLiteral(PS_STD_PLG)))
		return QStringLiteral(PS_STD_PLG);
	else
		throw PluginLoadException(QStringLiteral("No default plugin is defined or available"));
}
