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

public:
	explicit DatabaseController(QObject *parent = nullptr);

	static DatabaseController *instance();

	OperationQueue *operationQueue() const;
	ChangeFilter *changeFilter() const;

	QStringList listPackages() const;
	PackageInfo getInfo(const QString &pkgName) const;

	FilterInfo::Mode globalMode() const;
	QMap<QString, FilterInfo> filters() const;

	QString currentPath() const;
	void createDb(const QString &path, const QStringList &packages);
	void loadDb(const QString &path);
	void reloadDb();
	bool isLoaded() const;

	QVariant readSettings(const QString &key, const QVariant &defaultValue = QVariant()) const;
	void writeSettings(const QVariantHash &changes);

public slots:
	void setGlobalMode(FilterInfo::Mode mode);
	void setFilters(QMap<QString, FilterInfo> filters);

	void updateDb(const QStringList &packages);
	void sync();

signals:
	void operationsRequired(const QStringList &packagesInstall, const QStringList &packagesUninstall);

private slots:
	void fileChanged();

private:
	QSettings *_settings;
	OperationQueue *_opQueue;
	QString _dbPath;
	QJsonSerializer *_js;
	PackageDatabase _packageDatabase;
	QFileSystemWatcher *_watcher;
	bool _watcherSkipNext;
	bool _loaded;

	void cleanUp();
	void readFile();
	void writeFile(PackageDatabase p, const QString &path);
	void writeCurrentFile();
	QString lockPath(const QString &path);
	};

#endif // DATABASECONTROLLER_H
