#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <QObject>
#include <QPluginLoader>
#include "exception.h"
#include "packagemanagerplugin.h"

class PluginLoadException : public Exception
{
public:
	PluginLoadException(const QString &what);

	void raise() const final;
	QException *clone() const final;
};

class PluginLoader : public QObject
{
	Q_OBJECT

public:
	explicit PluginLoader(QObject *parent = nullptr);

	QStringList availablePlugins() const;
	PackageManagerPlugin *loadPlugin(const QString &name);

private:
	QHash<QString, QPluginLoader*> _availablePlugins;
};

#endif // PLUGINLOADER_H
