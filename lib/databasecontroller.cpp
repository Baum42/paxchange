#include "databasecontroller.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QGlobalStatic>
#include <QLockFile>

Q_GLOBAL_STATIC(DatabaseController, _instance)

static void setupDatabaseController();
Q_COREAPP_STARTUP_FUNCTION(setupDatabaseController)

DatabaseController::DatabaseController(QObject *parent) :
	QObject(parent),
	_settings(new QSettings()),
	_dbFile(new QFile()),
	_js(new QJsonSerializer(this)),
	_packageDatabase(),
	_watcher(new QFileSystemWatcher(this)),
	_loaded(false)
{
	_settings->beginGroup(QStringLiteral("lib/dbcontroller"));

	try {
		if(_settings->contains(QStringLiteral("path")))
			loadDb(_settings->value(QStringLiteral("path")).toString());
	} catch(QException &e) {
		qCritical() << e.what();
		cleanUp();
	}

	connect(_watcher, &QFileSystemWatcher::fileChanged,
			this, &DatabaseController::fileChanged);
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
{
	PackageDatabase p;
	foreach (auto package, packages)
		p.packages[package] = {package, false};

	QLockFile lock(lockPath(path));
	if(!lock.lock())
		throw DatabaseException("Lock failed");
	QFile file(path);
	if(!file.open(QIODevice::WriteOnly))
		throw DatabaseException(file.errorString());

	p.parseHarderToJson(_js);
	_js->serializeTo<PackageDatabase>(&file, p);
	file.close();
	lock.unlock();

	loadDb(path);
}

void DatabaseController::loadDb(const QString &path)
{
	cleanUp();

	_settings->setValue(QStringLiteral("path"), path);
	_dbFile->setFileName(path);

	readFile();

	_watcher->addPath(_dbFile->fileName());
	_loaded = true;
}

bool DatabaseController::isLoaded() const
{
	return _loaded;
}

QSettings::SettingsMap DatabaseController::readSettings() const
{
	Q_UNIMPLEMENTED();//TODO implement
}

void DatabaseController::writeSettings(const QSettings::SettingsMap &map)
{
	Q_UNIMPLEMENTED();//TODO implement
}

void DatabaseController::updateDb(const QStringList &packages)
{
	auto set = QSet<QString>::fromList(packages);
	for(auto it = _packageDatabase.packages.begin(); it != _packageDatabase.packages.end(); ) {
		if(!set.contains(it.key()))
			it = _packageDatabase.packages.erase(it);
		else{
			set.remove(it.key());
			it++;
		}
	}

	foreach (auto package, set)
		_packageDatabase.packages[package] = {package};

	QLockFile lock(lockPath(_dbFile->fileName()));
	if(!lock.lock())
		throw DatabaseException("Lock failed");
	if(!_dbFile->open(QIODevice::WriteOnly))
		throw DatabaseException(_dbFile->errorString());

	_packageDatabase.parseHarderToJson(_js);
	_js->serializeTo<PackageDatabase>(_dbFile, _packageDatabase);
	_dbFile->close();
	lock.unlock();
}

void DatabaseController::sync()
{
	QStringList pI, pUI;
	auto installedP = PluginLoader::plugin()->listAllPackages();
	auto targetP = _packageDatabase.packages;

	for(auto it = targetP.constBegin(); it != targetP.constEnd(); it++){
		auto contains = installedP.contains(it->name);
		if(!contains)
			pI.append(it->name);

		if(it->removed && contains)
			pUI.append(it->name);
	}

	operationsRequiered(pI, pUI);
}

void DatabaseController::fileChanged()
{
	try {
		readFile();
		sync();
	} catch(QException &e){
		qWarning() << "Failed to reload changed file:" << e.what();
		if(_dbFile->isOpen())
			_dbFile->close();
	}
}

void DatabaseController::cleanUp()
{
	if(_dbFile->isOpen())
		_dbFile->close();
	_packageDatabase = PackageDatabase();
	_watcher->removePaths(_watcher->files());
}

void DatabaseController::readFile()
{
	QLockFile lock(lockPath(_dbFile->fileName()));
	if(!lock.lock())
		throw DatabaseException("Lock failed");

	_dbFile->open(QIODevice::ReadOnly);
	auto pdb = _js->deserializeFrom<PackageDatabase>(_dbFile);
	_dbFile->close();
	lock.unlock();

	pdb.parseHarderFromJson(_js);
	_packageDatabase = pdb;
}

QString DatabaseController::lockPath(const QString &path)
{
	QFileInfo info(path);
	return info.absolutePath() + "." + info.fileName() + ".lock";
}

static void setupDatabaseController(){
	QJsonSerializer::registerListConverters<PackageInfo>();
}
