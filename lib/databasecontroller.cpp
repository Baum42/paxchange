#include "comboboxconfig.h"
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
	_opQueue(new OperationQueue(this)),
	_changeFilter(new ChangeFilter(this)),
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

	connect(PluginLoader::plugin(), &PackageManagerPlugin::packagesChanged,
			_changeFilter, &ChangeFilter::packagesChanged);
	connect(_changeFilter, &ChangeFilter::updateDatabase,
			this, &DatabaseController::updatePackages);

	try {
		if(_settings->contains(QStringLiteral("path"))) {
			loadDb(_settings->value(QStringLiteral("path")).toString());
			QMetaObject::invokeMethod(this, "sync", Qt::QueuedConnection);
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

FilterInfo::Mode DatabaseController::globalMode() const
{
	return _packageDatabase.globalMode;
}

QMap<QString, FilterInfo> DatabaseController::filters() const
{
	return _packageDatabase.filters;
}

QList<ExtraFilter> DatabaseController::extraFilters() const
{
	return _packageDatabase.extraFilters;
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

QVariant DatabaseController::readSettings(const QString &key, const QVariant &defaultValue) const
{
	auto res = _packageDatabase.settings.value(key).toVariant();
	if(res.isValid())
		return res;
	else
		return defaultValue;
}

void DatabaseController::writeSettings(const QVariantHash &changes)
{
	for(auto it = changes.constBegin(); it != changes.constEnd(); ++it) {
		if(it->isValid())
			_packageDatabase.settings.insert(it.key(), QJsonValue::fromVariant(it.value()));
		else
			_packageDatabase.settings.remove(it.key());
	}
	writeCurrentFile();
}

void DatabaseController::setGlobalMode(FilterInfo::Mode mode)
{
	_packageDatabase.globalMode = mode;
	writeCurrentFile();
}

void DatabaseController::setFilters(QMap<QString, FilterInfo> filters)
{
	_packageDatabase.filters = filters;
	writeCurrentFile();
}

void DatabaseController::setExtraFilters(QList<ExtraFilter> extraFilters)
{
	_packageDatabase.extraFilters = extraFilters;
	writeCurrentFile();
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

	writeCurrentFile();
}

void DatabaseController::sync()
{
	QStringList pI, pUI;
	auto installedP = PluginLoader::plugin()->listAllPackages();
	auto targetP = _packageDatabase.packages;

	for(auto it = targetP.constBegin(); it != targetP.constEnd(); it++){
		auto contains = installedP.contains(it->name);
		if(!contains && !it->removed)
			pI.append(it->name);

		if(it->removed && contains)
			pUI.append(it->name);
	}

	if(!(pI.isEmpty() && pUI.isEmpty()))
		emit operationsRequired(pI, pUI);
	if(!_packageDatabase.unclearPackages.isEmpty())
		emit unclearPackages(_packageDatabase.unclearPackages.values());
}

void DatabaseController::fileChanged()
{
	if(!_watcherSkipNext) {
		try {
			readFile();
			sync();
		} catch(QException &e){
			qWarning() << "Failed to reload changed file:" << e.what();
		}
	}
	_watcherSkipNext = false;

	_watcher->addPath(_dbPath);
}

void DatabaseController::updatePackages(const QList<PackageInfo> &addedPkg, const QList<UnclearPackageInfo> &unclearPkg)
{
	foreach (auto package, addedPkg)
		_packageDatabase.packages[package.name] = package;
	foreach (auto unclear, unclearPkg)
		_packageDatabase.unclearPackages[unclear.name] = unclear;
	writeCurrentFile();

	if(!_packageDatabase.unclearPackages.isEmpty())
		emit unclearPackages(_packageDatabase.unclearPackages.values());
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
		throw DatabaseException(QStringLiteral("Lock failed"));

	QFile file(_dbPath);
	if(!file.open(QIODevice::ReadOnly))
		throw DatabaseException(file.errorString());
	_packageDatabase = _js->deserializeFrom<PackageDatabase>(&file);
	file.close();
	lock.unlock();
}

void DatabaseController::writeFile(PackageDatabase p, const QString &path)
{
	QLockFile lock(lockPath(path));
	if(!lock.lock())
		throw DatabaseException(QStringLiteral("Lock failed"));
	QFile file(path);
	if(!file.open(QIODevice::WriteOnly))
		throw DatabaseException(file.errorString());

	_js->serializeTo<PackageDatabase>(&file, p);
	file.close();
	lock.unlock();

	_watcherSkipNext = true;
}

void DatabaseController::writeCurrentFile()
{
	writeFile(_packageDatabase, _dbPath);
}

QString DatabaseController::lockPath(const QString &path)
{
	QFileInfo info(path);
	return info.absolutePath() +
			QStringLiteral("/.") +
			info.fileName() +
			QStringLiteral(".lock");
}

static void setupDatabaseController()
{
	qRegisterMetaType<ComboboxConfig>();
	QJsonSerializer::registerAllConverters<PackageInfo>();
	QJsonSerializer::registerAllConverters<UnclearPackageInfo>();
	QJsonSerializer::registerAllConverters<QList<UnclearPackageInfo>>();
	QJsonSerializer::registerAllConverters<FilterInfo>();
	QJsonSerializer::registerAllConverters<ExtraFilter>();
}
