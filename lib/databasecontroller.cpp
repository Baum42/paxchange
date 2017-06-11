#include "databasecontroller.h"

#include <QCoreApplication>

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
	_settings->beginGroup(QStringLiteral("DatabaseController"));

	if(_settings->contains(QStringLiteral("path")))
		loadDb(_settings->value(QStringLiteral("path")).toString());
}

QStringList DatabaseController::listPackages() const
{
	QStringList list;
	foreach (auto p, _packageDatabase.packages) {
		list.append(p.name);
	}
	return list;
}

QString DatabaseController::currentPath() const
{
	return _dbFile->fileName();
}

void DatabaseController::createDb(const QString &path, const QStringList &packages)
{//TODO Exceptioon
	PackageDatabase p;
	foreach (auto package, packages) {
		p.packages.append({package, false});
	}

	QFile file(path);
	if(!file.open(QIODevice::WriteOnly))
		;//throw ..

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

	watcher->addPath(_dbFile->fileName());
}

void DatabaseController::updateDb(const QStringList &packages)
{
	/*foreach (auto package, packages) {//TODO use QHASH
		_packageDatabase.
	}*/
}

void DatabaseController::sync()
{

}

static void setupDatabaseController(){
	QJsonSerializer::registerListConverters<PackageInfo>();
}
