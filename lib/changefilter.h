#ifndef CHANGEFILTER_H
#define CHANGEFILTER_H

#include "packagedatabase.h"

#include <QObject>
#include <QRegularExpression>

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
	QList<PackageInfo> _pacInfoList;
	QList<UnclearPackageInfo> _uPacInfoList;
	QRegularExpression _re;

	void appendExtraFilter(QStringList &list, ExtraFilter filter, bool removed);
};

#endif // CHANGEFILTER_H
