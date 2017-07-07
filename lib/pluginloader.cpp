#include "pluginloader.h"
#include <QDir>
#include <QStandardPaths>
#include <QTemporaryFile>
#include <QJsonArray>
#include <QJsonValue>
#include <QDebug>
#include <QGlobalStatic>
#include <QDataStream>
#ifndef QT_NO_DEBUG
#include <QCoreApplication>
#else
#include <QLibraryInfo>
#endif

#include "databasecontroller.h"

Q_GLOBAL_STATIC(PluginLoader, pluginLoader)

PluginLoader::PluginLoader(QObject *parent) :
	QObject(parent),
	_availablePlugins(),
	_plugin(nullptr),
	_pluginKey()
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
			auto key = meta[QStringLiteral("MetaData")].toObject()[QStringLiteral("Key")].toString();
			if(!key.isEmpty()) {
				qInfo() << "Found plugin for key" << key;
				_availablePlugins.insert(key, loader);
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

QString PluginLoader::currentPlugin()
{
	return pluginLoader->_pluginKey;
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
		pluginLoader->_pluginKey = name;
		DatabaseController::loadTranslation(loader->metaData()[QStringLiteral("MetaData")].toObject()[QStringLiteral("TsPrefix")].toString());
		pluginLoader->_plugin->initialize();
	} else
		throw PluginLoadException(QStringLiteral("Failed to load plugin with error : %1")
								  .arg(loader->errorString()));
}

PackageManagerPlugin *PluginLoader::plugin()
{
	return pluginLoader->_plugin;
}

void PluginLoader::cacheForwardedPluginArgs(QStringList args)
{
	auto cacheDir = QDir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
	cacheDir.mkpath(QStringLiteral("hooks"));
	cacheDir.cd(QStringLiteral("hooks"));

	QTemporaryFile file(cacheDir.absoluteFilePath(QStringLiteral("XXXXXX.cmd")));
	file.setAutoRemove(false);
	file.open();
	QDataStream stream(&file);
	stream << args;
	file.close();
}

void PluginLoader::readCachedForwardedPluginArgs()
{
	auto cacheDir = QDir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
	if(cacheDir.cd(QStringLiteral("hooks"))){
		foreach (auto fileInfo, cacheDir.entryInfoList(QDir::Files)) {
			QFile file(fileInfo.absoluteFilePath());
			file.open(QIODevice::ReadOnly);
			QDataStream stream(&file);
			QStringList args;
			stream >> args;
			QMetaObject::invokeMethod(plugin(), "forwardedArguments",
									   Qt::QueuedConnection,
									   Q_ARG(QStringList, args));
			file.close();
			file.remove();
		}
	}
}

QString PluginLoader::defaultPlugin() const
{
	QSettings settings;
	auto preferred = settings.value(QStringLiteral("plugins/preferred"), QStringLiteral(PS_STD_PLG)).toString();
	if(_availablePlugins.contains(preferred))
		return preferred;
	else if(!_availablePlugins.isEmpty())
		return _availablePlugins.keys().first();
	else
		throw PluginLoadException(QStringLiteral("No plugins found"));
}
