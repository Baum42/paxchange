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
};

class LIBPACSYNC_SHARED_EXPORT PackageDatabase
{
	Q_GADGET

	Q_PROPERTY(Mode mode MEMBER mode)
	Q_PROPERTY(QMap<QString, PackageInfo> packages MEMBER packages)
	Q_PROPERTY(QJsonObject settings MEMBER settings)

public:
	enum Mode{
		BlackList,
		WhiteList
	};
	Q_ENUM(Mode)

	PackageDatabase();
	Mode mode;
	QMap<QString, PackageInfo> packages;

	QJsonObject settings;
};

#endif // PACKAGEDATABASE_H
