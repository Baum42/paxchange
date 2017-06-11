#ifndef PACKAGEDATABASE_H
#define PACKAGEDATABASE_H

#include <QObject>


class PackageInfo
{
	Q_GADGET

	Q_PROPERTY(QString name MEMBER name)
	Q_PROPERTY(bool conflict MEMBER conflict)

public:
	PackageInfo(QString name = "", bool conflict = false);
	QString name;
	bool conflict;

	bool operator ==(const PackageInfo &other) const;
};

class PackageDatabase
{
	Q_GADGET

	Q_PROPERTY(Mode mode MEMBER mode)
	Q_PROPERTY(QList<PackageInfo> packages MEMBER packages)

public:
	enum Mode{
		BlackList,
		WhiteList
	};
	Q_ENUM(Mode)

	PackageDatabase();
	Mode mode;
	QList<PackageInfo> packages;
};

#endif // PACKAGEDATABASE_H
