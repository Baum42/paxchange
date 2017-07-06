#ifndef PACKAGEDATABASE_H
#define PACKAGEDATABASE_H

#include "libpacsync_global.h"
#include <QtJsonSerializer/QJsonSerializer>
#include <QObject>

class LIBPACSYNC_SHARED_EXPORT PackageInfo
{
	Q_GADGET

	Q_PROPERTY(QString name MEMBER name)
	Q_PROPERTY(bool removed MEMBER removed)

public:
	PackageInfo(QString name = QString(), bool removed = false);
	QString name;
	bool removed;

	bool isValid() const;

	bool operator ==(const PackageInfo &other) const;
	bool equals(const PackageInfo &other) const;
};

class LIBPACSYNC_SHARED_EXPORT UnclearPackageInfo : public PackageInfo
{
	Q_GADGET

	Q_PROPERTY(QString hostName MEMBER hostName)
	Q_PROPERTY(QStringList filterNames MEMBER filterNames)

public:
	UnclearPackageInfo(QString name = {}, QString hostName = {}, QStringList filterNames = {});

	QString hostName;
	QStringList filterNames;

	bool operator ==(const UnclearPackageInfo &other) const;
	bool equals(const UnclearPackageInfo &other) const;
};

class LIBPACSYNC_SHARED_EXPORT FilterInfo
{
	Q_GADGET

	Q_PROPERTY(QString name MEMBER name)
	Q_PROPERTY(QString plugin MEMBER plugin)
	Q_PROPERTY(Mode mode MEMBER mode)
	Q_PROPERTY(QList<bool> pluginFilters MEMBER pluginFilters)
	Q_PROPERTY(QString regex MEMBER regex)

public:
	enum Mode {
		Add = 0,
		Skip = 1,
		Ask = 2
	};
	Q_ENUM(Mode)

public:
	FilterInfo(const QString &name = QString(),
			   const QString &plugin = QString());

	QString name;
	QString plugin;
	Mode mode;
	QList<bool> pluginFilters;
	QString regex;

	bool operator ==(const FilterInfo &other) const;
	bool equals(const FilterInfo &other) const;
};

class LIBPACSYNC_SHARED_EXPORT ExtraFilter
{
	Q_GADGET

	Q_PROPERTY(QString regex MEMBER regex)
	Q_PROPERTY(FilterInfo::Mode mode MEMBER mode)

public:
	ExtraFilter(QString regex = {}, FilterInfo::Mode mode = FilterInfo::Ask);

	QString regex;
	FilterInfo::Mode mode;

	bool operator ==(const ExtraFilter &other) const;
	bool equals(const ExtraFilter &other) const;
};

class LIBPACSYNC_SHARED_EXPORT PackageDatabase
{
	Q_GADGET

	Q_PROPERTY(QMap<QString, PackageInfo> packages MEMBER packages)
	Q_PROPERTY(QMap<QString, UnclearPackageInfo> unclearPackages MEMBER unclearPackages)
	Q_PROPERTY(FilterInfo::Mode globalMode MEMBER globalMode)
	Q_PROPERTY(QMap<QString, FilterInfo> filters MEMBER filters)
	Q_PROPERTY(QList<ExtraFilter> extraFilters MEMBER extraFilters)
	Q_PROPERTY(QJsonObject settings MEMBER settings)

public:
	PackageDatabase();

	QMap<QString, PackageInfo> packages;//key: PackageInfo::name
	QMap<QString, UnclearPackageInfo> unclearPackages;//key: UnclearPackageInfo::name

	FilterInfo::Mode globalMode;
	QMap<QString, FilterInfo> filters;//key: FilterInfo::name
	QList<ExtraFilter> extraFilters;

	QJsonObject settings;
};

Q_DECLARE_METATYPE(PackageInfo)
Q_DECLARE_METATYPE(UnclearPackageInfo)
Q_DECLARE_METATYPE(FilterInfo)
Q_DECLARE_METATYPE(ExtraFilter)
Q_DECLARE_METATYPE(PackageDatabase)

#endif // PACKAGEDATABASE_H
