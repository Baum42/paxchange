#ifndef DATABASECONTROLLER_H
#define DATABASECONTROLLER_H

#include "libpacsync_global.h"
#include "packagedatabase.h"
#include "pluginloader.h"
#include "operationqueue.h"
#include "changefilter.h"

#include <QFile>
#include <QObject>
#include <QSettings>
#include <QtJsonSerializer/QJsonSerializer>
#include <QFileSystemWatcher>

DEF_EXC(DatabaseException)

class LIBPACSYNC_SHARED_EXPORT DatabaseController : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QStringList packageList READ listPackages WRITE updateDb)
	Q_PROPERTY(FilterInfo::Mode globalMode READ globalMode WRITE setGlobalMode)
	Q_PROPERTY(QMap<QString, FilterInfo> filters READ filters WRITE setFilters)
	Q_PROPERTY(QList<ExtraFilter> extraFilters READ extraFilters WRITE setExtraFilters)

public:
	explicit DatabaseController(QObject *parent = nullptr);

	static DatabaseController *instance();

	OperationQueue *operationQueue() const;

	QStringList listPackages() const;
	QList<UnclearPackageInfo> listUnclearPackages() const;
	PackageInfo getInfo(const QString &pkgName) const;

	FilterInfo::Mode globalMode() const;
	QMap<QString, FilterInfo> filters() const;
	QList<ExtraFilter> extraFilters() const;

	QString currentPath() const;
	void createDb(const QString &path, const QStringList &packages);
	void loadDb(const QString &path);
	void reloadDb();
	bool isLoaded() const;

	QVariant readSettings(const QString &key, const QVariant &defaultValue = QVariant()) const;
	void writeSettings(const QVariantHash &changes);

	void beginSaveTransaction();
	void commitSave();

public slots:
	void setGlobalMode(FilterInfo::Mode mode);
	void setFilters(QMap<QString, FilterInfo> filters);
	void setExtraFilters(QList<ExtraFilter> extraFilters);

	void updateDb(const QStringList &packages);
	void clearPackages(const QList<UnclearPackageInfo> &clearedPackages);

	void sync();

signals:
	void operationsRequired(const QStringList &packagesInstall, const QStringList &packagesUninstall);
	void unclearPackagesChanged(int count);

	void guiError(const QString &error, bool critical = false);

private slots:
	void fileChanged();
	void updatePackages(const QList<PackageInfo> &addedPkg, const QList<UnclearPackageInfo> &unclearPkg);

	void syncImpl();

private:
	QSettings *_settings;
	OperationQueue *_opQueue;
	ChangeFilter *_changeFilter;
	QString _dbPath;
	QJsonSerializer *_js;
	PackageDatabase _packageDatabase;
	QFileSystemWatcher *_watcher;
	bool _watcherSkipNext;
	bool _loaded;

	bool _isTransaction;

	void cleanUp();
	void readFile();
	void writeFile(PackageDatabase p, const QString &path);
	void writeCurrentFile();
	QString lockPath(const QString &path);
	};

#endif // DATABASECONTROLLER_H
