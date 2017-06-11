#include "databasecontroller.h"

#include <QCoreApplication>
#include <QGlobalStatic>

Q_GLOBAL_STATIC(DatabaseController, _instance)

static void setupDatabaseController();
Q_COREAPP_STARTUP_FUNCTION(setupDatabaseController)

DatabaseController::DatabaseController(QObject *parent) :
	QObject(parent),
	_settings(new QSettings()),
	_dbFile(new QFile()),
	_js(new QJsonSerializer(this)),
	_packageDatabase(),
	watcher(new QFileSystemWatcher(this))
{
	_settings->beginGroup(QStringLiteral("lib/dbcontroller"));

	if(_settings->contains(QStringLiteral("path")))
		loadDb(_settings->value(QStringLiteral("path")).toString());
}

DatabaseController *DatabaseController::instance()
{
	return _instance;
}

QStringList DatabaseController::listPackages() const
{
	return _packageDatabase.packages.keys();
}

QString DatabaseController::currentPath() const
{
	return _dbFile->fileName();
}

void DatabaseController::createDb(const QString &path, const QStringList &packages)
{//TODO Exceptioon
	PackageDatabase p;
	foreach (auto package, packages) {
		p.packages[package] = {package, false};
	}

	QFile file(path);
	if(!file.open(QIODevice::WriteOnly))
		;//throw ..

	p.parseHarderToJson(_js);
	_js->serializeTo<PackageDatabase>(&file, p);
	file.close();

	loadDb(path);
}

void DatabaseController::loadDb(const QString &path)
{//TODO Exceptioon
	watcher->deleteLater();
	watcher = new QFileSystemWatcher(this);

	_settings->setValue(QStringLiteral("path"), path);
	_dbFile->setFileName(path);

	_packageDatabase = _js->deserializeFrom<PackageDatabase>(_dbFile);
	_packageDatabase.parseHarderFromJson(_js);//TODO use QHash later

	watcher->addPath(_dbFile->fileName());
}

void DatabaseController::updateDb(const QStringList &packages)
{
	/*foreach (auto package, packages) {
		if(_packageDatabase.packages.contains(package))

	}*/
}

void DatabaseController::sync()
{

}

static void setupDatabaseController(){
	QJsonSerializer::registerListConverters<PackageInfo>();
}
