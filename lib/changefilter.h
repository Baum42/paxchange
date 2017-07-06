#ifndef CHANGEFILTER_H
#define CHANGEFILTER_H

#include "packagedatabase.h"

#include <QObject>
#include <QRegularExpression>
#include <QMap>
#include <QHash>
#include <functional>

class ChangeFilter : public QObject
{
	Q_OBJECT

public:
	explicit ChangeFilter(QObject *parent = nullptr);

public slots:
	void packagesChanged(const QStringList &added, const QStringList &removed);

signals:
	void updateDatabase(const QList<PackageInfo> &addedPkg, const QList<UnclearPackageInfo> &unclearPkg);

private:
	QMap<QString, PackageInfo> _pacInfoList;
	QMap<QString, UnclearPackageInfo> _uPacInfoList;

	bool setRegexPattern(QString pattern);
	void addPacInfo(ExtraFilter extraFilter, bool alreadyAdded);

	template <typename TFilter>
	void applyFilters(const QSet<QString> &packages,
					  const QList<TFilter> &filters,
					  const std::function<bool(QString, TFilter)> &filterFn,
					  const std::function<QString(TFilter)> &filterNameFn);
};

#endif // CHANGEFILTER_H
