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
	PackageInfo(QString name = "", bool removed = false);
	QString name;
	bool removed;

	bool operator ==(const PackageInfo &other) const;
};

class LIBPACSYNC_SHARED_EXPORT PackageDatabase
{
	Q_GADGET

	Q_PROPERTY(Mode mode MEMBER mode)
	Q_PROPERTY(QJsonObject packages MEMBER _pkg)
	Q_PROPERTY(QJsonObject settings MEMBER settings)

public:
	enum Mode{
		BlackList,
		WhiteList
	};
	Q_ENUM(Mode)

	PackageDatabase();
	Mode mode;
	QHash<QString, PackageInfo> packages;

	QJsonObject settings;

	void parseHarderFromJson(QJsonSerializer *s);//TODO use QHash later
	void parseHarderToJson(QJsonSerializer *s);//TODO use QHash later

private:
	QJsonObject _pkg;
};

#endif // PACKAGEDATABASE_H
