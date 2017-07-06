#ifndef CHANGEFILTER_H
#define CHANGEFILTER_H

#include "packagedatabase.h"

#include <QObject>
#include <QRegularExpression>
#include <QMap>

class ChangeFilter : public QObject
{
	Q_OBJECT

public:
	explicit ChangeFilter(QObject *parent = nullptr);

public slots:
	void packagesChanged(QStringList added, QStringList removed);

signals:
	void updateDatabase(const QList<PackageInfo> &infos);
	void packagesUnclear(const QList<UnclearPackageInfo> &infos);

private:
	QMap<QString, PackageInfo> _pacInfoList;
	QMap<QString, UnclearPackageInfo> _uPacInfoList;
	QRegularExpression _re;

	bool setRegexPattern(QString pattern);
	void addPacInfo(ExtraFilter extraFilter, bool alreadyAdded);
};

#endif // CHANGEFILTER_H
