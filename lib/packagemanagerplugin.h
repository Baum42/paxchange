#ifndef PACKAGEMANAGERPLUGIN_H
#define PACKAGEMANAGERPLUGIN_H

#include "libpaxchange_global.h"

#include <QObject>
#include <QSettings>
#include <QVariant>
#include <QVector>

#include "syncedsettings.h"

#define PackageManagerPlugin_iid "de.baum42.paxchange.PackageManagerPlugin"

class LIBPAXCHANGE_SHARED_EXPORT PackageManagerPlugin : public QObject
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
		QVariantMap widgetProperties;
		SettingsInfo(QString displayName,
					 QString description,
					 QString settingsKeys,
					 int type = QMetaType::QString,
					 QVariant defaultValue = {},
					 QByteArray widgetClassName = {},
					 QVariantMap widgetProperties = {});
	};

	PackageManagerPlugin(QObject *parent = nullptr);

	virtual void initialize();

	virtual QList<FilterInfo> extraFilters() = 0;

	virtual QStringList listAllPackages() = 0;
	virtual QStringList listPackages(QVector<bool> extraFilters) = 0;
	virtual QString installationCmd(const QStringList &packages) = 0;
	virtual QString uninstallationCmd(const QStringList &packages) = 0;
	virtual bool startGuiInstall(const QStringList &packages);
	virtual bool startGuiUninstall(const QStringList &packages);

	QSettings *createLocalSettings(QObject *parent) const;
	virtual QList<SettingsInfo> listSettings() const = 0;
	virtual void settingsChanged();

public slots:
	virtual void forwardedArguments(QStringList args);

signals:
	void guiFinished();
	void packagesChanged(const QStringList &added, const QStringList &removed);
};

#endif // PACKAGEMANAGERPLUGIN_H
