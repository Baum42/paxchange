#include "pluginloader.h"
#include <QDir>
#include <QStandardPaths>
#include <QTemporaryFile>
#include <QJsonArray>
#include <QJsonValue>
#include <QDebug>
#include <QGlobalStatic>
#include <QDataStream>
#include <QCoreApplication>
#ifdef QT_NO_DEBUG
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
	path += QStringLiteral("/paxchange");

	QDir plugDir(path);
	plugDir.setFilter(QDir::Files | QDir::Readable);
	for (auto plg : plugDir.entryList()) {
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

bool PluginLoader::cacheForwardedPluginArgs(QStringList args)
{
	QDir cacheDir(QStringLiteral("/var/cache/paxchange"));

	if(!cacheDir.exists()) {
		if(!cacheDir.mkpath(QStringLiteral("."))) {
			qCritical().noquote() << "Failed to create cache directory" << cacheDir.absolutePath();
			return false;
		}
		QFile::setPermissions(cacheDir.absolutePath(),
							  QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner |
							  QFileDevice::ReadGroup | QFileDevice::WriteGroup | QFileDevice::ExeGroup |
							  QFileDevice::ReadOther | QFileDevice::WriteOther | QFileDevice::ExeOther);
	}

	QFile file(cacheDir.absoluteFilePath(QStringLiteral("hooks.cache")));
	auto overwrite = file.exists();
	if(file.open(QIODevice::WriteOnly)) {
		if(overwrite)
			qWarning() << "Overwriting previously chached package changes!";

		QDataStream stream(&file);
		stream << args;
		file.close();

		qInfo().noquote() << "Cached packages changes because"
						  << QCoreApplication::applicationName()
						  << "is not running. Run"
						  << QCoreApplication::applicationName()
						  << "before the next package change to react to these changes.";
		return true;
	} else {
		qCritical().noquote() << "Failed to cache packages changes with error:"
							  << file.errorString();
		return false;
	}
}

void PluginLoader::readCachedForwardedPluginArgs()
{
	QDir cacheDir(QStringLiteral("/var/cache/paxchange"));

	QFile file(cacheDir.absoluteFilePath(QStringLiteral("hooks.cache")));
	if(!file.exists())
		return;

	if(file.open(QIODevice::ReadOnly)) {
		QDataStream stream(&file);
		QStringList args;
		stream >> args;
		QMetaObject::invokeMethod(plugin(), "forwardedArguments",
								   Qt::QueuedConnection,
								   Q_ARG(QStringList, args));
		file.close();
		if(!file.remove())
			qWarning() << "Failed to remove cache file:" << file.fileName();
	} else {
		qCritical().noquote() << "Failed to read cached packages changes with error:"
							  << file.errorString();
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
