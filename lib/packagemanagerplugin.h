#ifndef PACKAGEMANAGERPLUGIN_H
#define PACKAGEMANAGERPLUGIN_H

#include "lib_global.h"

#include <QObject>

#define PackageManagerPlugin_iid "de.baum42.pacsync.PackageManagerPlugin"

class LIBSHARED_EXPORT PackageManagerPlugin : public QObject
{
	Q_OBJECT

public:
	struct FilterInfo {
		QString text;
		QString toolTip;
		bool defaultValue;
	};

	PackageManagerPlugin(QObject *parent = nullptr);

	virtual QList<FilterInfo> extraFilters() = 0;

	virtual QStringList listAllPackages() = 0;
	virtual QStringList listPackages(QList<bool> extraFilters) = 0;//TODO QVector
	virtual void startInstallation(const QStringList &packages, bool noConfirm) = 0;
	virtual void startUninstallation(const QStringList &packages, bool noConfirm) = 0;

signals:
	void operationCompleted();
	void packagesChanged(const QStringList &added, const QStringList &removed);
};

#endif // PACKAGEMANAGERPLUGIN_H
