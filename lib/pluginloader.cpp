#include "pluginloader.h"
#include <QDir>
#include <QJsonArray>
#ifndef QT_NO_DEBUG
#include <QCoreApplication>
#else
#include <QLibraryInfo>
#endif

PluginLoader::PluginLoader(QObject *parent) :
	QObject(parent)
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
		if(meta["iid"].toString() == PackageManagerPlugin_iid) {
			auto keys = meta["meta"].toObject()["keys"].toArray();
			if(!keys.isEmpty()) {
				foreach (auto key, keys)
					_availablePlugins.insert(key, loader);
				continue;
			}
		}

		loader->deleteLater();
	}
}

QStringList PluginLoader::availablePlugins() const
{
	return _availablePlugins.keys();
}

PackageManagerPlugin *PluginLoader::loadPlugin(const QString &name)
{
	auto loader = _availablePlugins.value(name, nullptr);
	if(!loader)
		throw PluginLoadException(QStringLiteral("The given key does not name a plugin"));

	if(loader->load()) {
		auto object = loader->instance();
		auto plugin = qobject_cast<PackageManagerPlugin*>(object);
		if(plugin)
			return plugin;
		else
			throw PluginLoadException(QStringLiteral("The loaded plugin is not a PackageManagerPlugin"));
	} else
		throw PluginLoadException(QStringLiteral("Failed to load plugin with error : %1")
								  .arg(loader->errorString()));
}

PluginLoadException::PluginLoadException(const QString &what) :
	Exception(what)
{}

void PluginLoadException::raise() const
{
	throw *this;
}

QException *PluginLoadException::clone() const
{
	auto e = new PluginLoadException(QString());
	e->_what = _what;
	return e;
}
