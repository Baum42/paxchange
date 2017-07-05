#ifndef CHANGEFILTER_H
#define CHANGEFILTER_H

#include "packagedatabase.h"

#include <QObject>

class ChangeFilter : public QObject
{
	Q_OBJECT

public:
	explicit ChangeFilter(QObject *parent = nullptr);

public slots:
	void packagesChanged(const QStringList &added, const QStringList &removed);

signals:
	void updateDatabase(const QList<PackageInfo> &infos);
	void packagesUnclear(const QList<UnclearPackageInfo> &infos);
};

#endif // CHANGEFILTER_H
