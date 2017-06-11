#include "databasecontroller.h"

#include <QCoreApplication>

static void setupDatabaseController();
Q_COREAPP_STARTUP_FUNCTION(setupDatabaseController)

DatabaseController::DatabaseController(QObject *parent) :
	QObject(parent),
	_settings(new QSettings()),
	_dbFile(new QFile()),
	_js(new QJsonSerializer(this)),
	_packageDatabase()
{
	_settings->beginGroup(QStringLiteral("DatabaseController"));
	//TODOload db
}

QStringList DatabaseController::listPackages() const
{

}

QString DatabaseController::currentPath() const
{
	return _dbFile->fileName();
}

void DatabaseController::createDb(const QString &path, const QStringList &packages)
{
	removeOldDb();

	_settings->setValue(QStringLiteral("path"), path);
	_dbFile->setFileName(path);

	_packageDatabase = _js->deserializeFrom<PackageDatabase>(_dbFile);
}

void DatabaseController::loadDb(const QString &path)
{

}

void DatabaseController::updateDb(const QStringList &packages)
{

}

void DatabaseController::removeOldDb()
{

}

static void setupDatabaseController(){
	QJsonSerializer::registerListConverters<PackageInfo>();
}
