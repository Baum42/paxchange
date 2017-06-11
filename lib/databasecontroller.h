#ifndef DATABASECONTROLLER_H
#define DATABASECONTROLLER_H

#include "packagedatabase.h"

#include <QFile>
#include <QObject>
#include <QSettings>
#include <QJsonSerializer>

class DatabaseController : public QObject
{
	Q_OBJECT
public:
	explicit DatabaseController(QObject *parent = nullptr);

	QStringList listPackages() const;
	QString currentPath() const;

public slots:
	void createDb(const QString &path, const QStringList &packages);
	void loadDb(const QString &path);
	void updateDb(const QStringList &packages);

signals:
	void syncCheck(const QStringList &packages);

private:
	QSettings *_settings;
	QFile *_dbFile;
	QJsonSerializer *_js;
	PackageDatabase _packageDatabase;

	void removeOldDb();
};

#endif // DATABASECONTROLLER_H
