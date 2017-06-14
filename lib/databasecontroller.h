#ifndef DATABASECONTROLLER_H
#define DATABASECONTROLLER_H

#include "libpacsync_global.h"
#include "packagedatabase.h"
#include "pluginloader.h"
#include "operationqueue.h"

#include <QFile>
#include <QObject>
#include <QSettings>
#include <QtJsonSerializer/QJsonSerializer>
#include <QFileSystemWatcher>

DEF_EXC(DatabaseException)

class LIBPACSYNC_SHARED_EXPORT DatabaseController : public QObject
{
	Q_OBJECT

public:
	explicit DatabaseController(QObject *parent = nullptr);

	static DatabaseController *instance();

	OperationQueue *operationQueue() const;

	QStringList listPackages() const;//TODO all data (color code)
	QString currentPath() const;
	void createDb(const QString &path, const QStringList &packages);
	void loadDb(const QString &path);
	void reloadDb();
	bool isLoaded() const;

	QSettings::SettingsMap readSettings() const;
	void writeSettings(const QSettings::SettingsMap &map);

public slots:
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
	QString lockPath(const QString &path);
	};

#endif // DATABASECONTROLLER_H
