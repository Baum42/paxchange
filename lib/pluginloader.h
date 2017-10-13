#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include "libpaxchange_global.h"
#include <QObject>
#include <QPluginLoader>
#include "exception.h"
#include "packagemanagerplugin.h"

DEF_EXC(PluginLoadException)

class LIBPAXCHANGE_SHARED_EXPORT PluginLoader : public QObject
{
	Q_OBJECT

public:
	explicit PluginLoader(QObject *parent = nullptr);

	static QStringList availablePlugins();
	static QString currentPlugin();
	static void loadPlugin(const QString &overwrite);

	static PackageManagerPlugin *plugin();

	static bool cacheForwardedPluginArgs(QStringList args);
	static void readCachedForwardedPluginArgs();

private:
	QHash<QString, QPluginLoader*> _availablePlugins;
	PackageManagerPlugin *_plugin;
	QString _pluginKey;

	QString defaultPlugin() const;
};

#endif // PLUGINLOADER_H
