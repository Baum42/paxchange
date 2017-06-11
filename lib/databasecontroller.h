#ifndef DATABASECONTROLLER_H
#define DATABASECONTROLLER_H

#include "packagedatabase.h"

#include <QFile>
#include <QObject>
#include <QSettings>
#include <QtJsonSerializer/QJsonSerializer>
#include <QFileSystemWatcher>

class DatabaseController : public QObject
{
	Q_OBJECT

public:
	explicit DatabaseController(QObject *parent = nullptr);

	static DatabaseController *instance();

	QStringList listPackages() const;
	QString currentPath() const;
	void createDb(const QString &path, const QStringList &packages);
	void loadDb(const QString &path);

public slots:
	void updateDb(const QStringList &packages);
	void sync();

signals:
	void syncCheck(const QStringList &packages);

private:
	QSettings *_settings;
	QFile *_dbFile;
	QJsonSerializer *_js;
	PackageDatabase _packageDatabase;
	QFileSystemWatcher *watcher;
};

#endif // DATABASECONTROLLER_H
