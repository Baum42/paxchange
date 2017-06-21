#ifndef PACKAGEMANAGERPLUGIN_H
#define PACKAGEMANAGERPLUGIN_H

#include "libpacsync_global.h"

#include <QObject>
#include <QSettings>
#include <QVariant>

#define PackageManagerPlugin_iid "de.baum42.pacsync.PackageManagerPlugin"

class LIBPACSYNC_SHARED_EXPORT PackageManagerPlugin : public QObject
{
	Q_OBJECT

public:
	struct FilterInfo {
		QString text;
		QString toolTip;
		bool defaultValue;
	};

	struct SettingsInfo {
		QString displayName;
		QString description;
		QString settingsKeys;
		int type; //QMetaType::Type
		QVariant defaultValue;
		QByteArray widgetClassName;
	};

	PackageManagerPlugin(QObject *parent = nullptr);

	virtual QList<FilterInfo> extraFilters() = 0;

	virtual QStringList listAllPackages() = 0;
	virtual QStringList listPackages(QList<bool> extraFilters) = 0;//TODO QVector
	virtual void startInstallation(const QStringList &packages) = 0;
	virtual void startUninstallation(const QStringList &packages) = 0;

	QSettings *createPluginSettings(QObject *parent) const;
	virtual QList<SettingsInfo> listSettings() = 0;
	virtual void settingsChanged();//TODO call

signals:
	void operationCompleted();
	void packagesChanged(const QStringList &added, const QStringList &removed);
};

#endif // PACKAGEMANAGERPLUGIN_H
