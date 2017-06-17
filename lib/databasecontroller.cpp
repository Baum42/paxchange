#include "databasecontroller.h"
#include "dbsettings.h"

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
	_opQueue(new OperationQueue(this)),
	_dbPath(),
	_js(new QJsonSerializer(this)),
	_packageDatabase(),
	_watcher(new QFileSystemWatcher(this)),
	_watcherSkipNext(false),
	_loaded(false)
{
	_settings->beginGroup(QStringLiteral("lib/dbcontroller"));

	connect(this, &DatabaseController::operationsRequired,
			_opQueue, &OperationQueue::setOperations);

	try {
		if(_settings->contains(QStringLiteral("path"))) {
			loadDb(_settings->value(QStringLiteral("path")).toString());
			sync();
		}
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

OperationQueue *DatabaseController::operationQueue() const
{
	return _opQueue;
}

QStringList DatabaseController::listPackages() const
{
	return _packageDatabase.packages.keys();
}

PackageInfo DatabaseController::getInfo(const QString &pkgName) const
{
	return _packageDatabase.packages.value(pkgName);
}

QString DatabaseController::currentPath() const
{
	return _dbPath;
}

void DatabaseController::createDb(const QString &path, const QStringList &packages)
{
	PackageDatabase p;
	foreach (auto package, packages)
		p.packages[package] = {package, false};

	writeFile(p, path);

	loadDb(path);
}

void DatabaseController::loadDb(const QString &path)
{
	cleanUp();

	_settings->setValue(QStringLiteral("path"), path);
	_dbPath = path;

	readFile();

	_watcher->addPath(_dbPath);
	_loaded = true;
}

void DatabaseController::reloadDb()
{
	readFile();
}

bool DatabaseController::isLoaded() const
{
	return _loaded;
}

QSettings::SettingsMap DatabaseController::readSettings() const
{
	return QJsonValue(_packageDatabase.settings).toVariant().toMap();
}

void DatabaseController::writeSettings(const QSettings::SettingsMap &map)
{
	_packageDatabase.settings = QJsonValue::fromVariant(map).toObject();

	writeFile(_packageDatabase, _dbPath);
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

	writeFile(_packageDatabase, _dbPath);
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

	if(!(pI.isEmpty() && pUI.isEmpty()))
		emit operationsRequired(pI, pUI);
}

void DatabaseController::fileChanged()
{
	if(!_watcherSkipNext) {
		try {
			readFile();
			sync();
			DbSettings::triggerChange();
		} catch(QException &e){
			qWarning() << "Failed to reload changed file:" << e.what();
		}
	}
	_watcherSkipNext = false;

	_watcher->addPath(_dbPath);
}

void DatabaseController::cleanUp()
{
	_packageDatabase = PackageDatabase();
	_watcher->removePaths(_watcher->files());
}

void DatabaseController::readFile()
{
	QLockFile lock(lockPath(_dbPath));
	if(!lock.lock())
		throw DatabaseException("Lock failed");

	QFile file(_dbPath);
	file.open(QIODevice::ReadOnly);
	_packageDatabase = _js->deserializeFrom<PackageDatabase>(&file);
	file.close();
	lock.unlock();
}

void DatabaseController::writeFile(PackageDatabase p, const QString &path)
{
	QLockFile lock(lockPath(path));
	if(!lock.lock())
		throw DatabaseException("Lock failed");
	QFile file(path);
	if(!file.open(QIODevice::WriteOnly))
		throw DatabaseException(file.errorString());

	_js->serializeTo<PackageDatabase>(&file, p);
	file.close();
	lock.unlock();

	_watcherSkipNext = true;
}

QString DatabaseController::lockPath(const QString &path)
{
	QFileInfo info(path);
	return info.absolutePath() + "/." + info.fileName() + ".lock";
}

static void setupDatabaseController()
{
	QJsonSerializer::registerAllConverters<PackageInfo>();
	DbSettings::registerSettings();
}
