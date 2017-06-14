#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <QObject>
#include <QPluginLoader>
#include "exception.h"
#include "packagemanagerplugin.h"

DEF_EXC(PluginLoadException)

class PluginLoader : public QObject
{
	Q_OBJECT

public:
	explicit PluginLoader(QObject *parent = nullptr);

	static QStringList availablePlugins();
	static void loadPlugin(const QString &overwrite);

	static PackageManagerPlugin *plugin();

private:
	QHash<QString, QPluginLoader*> _availablePlugins;
	PackageManagerPlugin *_plugin;

	QString defaultPlugin() const;
};

#endif // PLUGINLOADER_H
