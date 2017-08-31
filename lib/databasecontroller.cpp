#include "comboboxconfig.h"
#include "databasecontroller.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QGlobalStatic>
#include <QLockFile>
#include <QTranslator>
#include <QLibraryInfo>

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
	_reloadTimer(new QTimer(this)),
	_watcherSkipNext(false),
	_loaded(false),
	_isTransaction(false)
{
	_settings->beginGroup(QStringLiteral("lib/dbcontroller"));

	_reloadTimer->setTimerType(Qt::VeryCoarseTimer);
	connect(_reloadTimer, &QTimer::timeout,
			this, &DatabaseController::reloadDb);

	connect(this, &DatabaseController::operationsRequired,
			_opQueue, &OperationQueue::setOperations);

	connect(PluginLoader::plugin(), &PackageManagerPlugin::packagesChanged,
			_changeFilter, &ChangeFilter::packagesChanged);
	connect(_changeFilter, &ChangeFilter::updateDatabase,
			this, &DatabaseController::updatePackages);

	try {
		if(_settings->contains(QStringLiteral("path"))) {
			loadDb(_settings->value(QStringLiteral("path")).toString());
			sync();
		}
	} catch(QException &e) {
		qCritical() << "Load failed:" << e.what();
		cleanUp();
		QMetaObject::invokeMethod(this, "guiError", Qt::QueuedConnection,
								  Q_ARG(QString, tr("Failed to load last database file!")),
								  Q_ARG(bool, true));
		return;
	}

	connect(_watcher, &QFileSystemWatcher::fileChanged,
			this, &DatabaseController::fileChanged);
}

void DatabaseController::loadTranslation(const QString &name)
{
	auto translator = new QTranslator(qApp);
	if(translator->load(QLocale(),
						name,
						QStringLiteral("_"),
						QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
		qApp->installTranslator(translator);
	else {
		qWarning() << "Failed to load translations for" << name;
		delete translator;
	}
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

QList<UnclearPackageInfo> DatabaseController::listUnclearPackages() const
{
	return _packageDatabase.unclearPackages.values();
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

	_dbPath = path;
	readFile();
	_settings->setValue(QStringLiteral("path"), path);

	_watcher->addPath(_dbPath);
	_loaded = true;
}

void DatabaseController::reloadDb()
{
	readFile();
	sync();
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

void DatabaseController::beginSaveTransaction()
{
	_isTransaction = true;
}

void DatabaseController::commitSave()
{
	if(_isTransaction) {
		_isTransaction = false;
		writeCurrentFile();
	}
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

void DatabaseController::clearPackages(const QList<UnclearPackageInfo> &clearedPackages)
{
	foreach (auto package, clearedPackages)
		_packageDatabase.packages[package.name] = package;
	_packageDatabase.unclearPackages.clear();
	writeCurrentFile();
	sync();
}

void DatabaseController::sync()
{
	QMetaObject::invokeMethod(this, "syncImpl", Qt::QueuedConnection);
}

void DatabaseController::fileChanged()
{
	if(!_watcherSkipNext) {
		try {
			readFile();
			sync();
		} catch(QException &e){
			qWarning() << "Failed to reload changed file:" << e.what();
			emit guiError(tr("Failed to reload changed package database"));
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
	emit unclearPackagesChanged(_packageDatabase.unclearPackages.size());
}

void DatabaseController::syncImpl()
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

	auto interval = readSettings(QStringLiteral("lib/database/resync"), 0).toInt();
	if(interval == 0)
		_reloadTimer->stop();
	else
		_reloadTimer->start(interval * 60 * 1000);

	emit operationsRequired(pI, pUI);
	emit unclearPackagesChanged(_packageDatabase.unclearPackages.size());
}

void DatabaseController::cleanUp()
{
	_packageDatabase = PackageDatabase();
	_watcher->removePaths(_watcher->files());
}

void DatabaseController::readFile()
{
	QFile file(_dbPath);
	if(!file.exists())
		throw DatabaseException(QStringLiteral("Database file does not exist"));

	QLockFile lock(lockPath(_dbPath));
	if(!lock.lock())
		throw DatabaseException(QStringLiteral("Lock failed"));

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
	if(!_isTransaction) {
		try {
			writeFile(_packageDatabase, _dbPath);
		} catch(QException &e) {
			qWarning() << "Failed to save database:" << e.what();
			emit guiError(tr("Failed to save package database!"), true);
		}
	}
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

#ifdef Q_OS_WIN
	qSetMessagePattern(QStringLiteral("%{if-debug}[Debug]    %{endif}"
									  "%{if-info}[Info]     %{endif}"
									  "%{if-warning}[Warning]  %{endif}"
									  "%{if-critical}[Critical] %{endif}"
									  "%{if-fatal}[Fatal]    %{endif}"
									  "%{if-category}%{category}: %{endif}"
									  "%{message}"));
#else
	qSetMessagePattern(QStringLiteral("%{if-debug}[\033[32mDebug\033[0m]    %{endif}"
									  "%{if-info}[\033[36mInfo\033[0m]     %{endif}"
									  "%{if-warning}[\033[33mWarning\033[0m]  %{endif}"
									  "%{if-critical}[\033[31mCritical\033[0m] %{endif}"
									  "%{if-fatal}[\033[35mFatal\033[0m]    %{endif}"
									  "%{if-category}%{category}: %{endif}"
									  "%{message}"));
#endif

	//load translations
	DatabaseController::loadTranslation(QStringLiteral("paxchange_lib"));
}
