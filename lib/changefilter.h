#ifndef CHANGEFILTER_H
#define CHANGEFILTER_H

#include "libpaxchange_global.h"
#include "packagedatabase.h"

#include <QObject>
#include <QRegularExpression>
#include <QMap>
#include <QHash>
#include <functional>

class LIBPAXCHANGE_SHARED_EXPORT ChangeFilter : public QObject
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

	QRegularExpression createRegEx(QString pattern);

	template <typename TFilter>
	void applyFilters(QSet<QString> &packages,
					  const QList<TFilter> &filters,
					  const std::function<bool(QString, TFilter)> &filterFn,
					  const std::function<QString(TFilter)> &filterNameFn);
};

#endif // CHANGEFILTER_H
