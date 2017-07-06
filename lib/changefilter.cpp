#include "changefilter.h"
#include "databasecontroller.h"


ChangeFilter::ChangeFilter(QObject *parent) :
	QObject(parent)
{}

void ChangeFilter::packagesChanged(const QStringList &added, const QStringList &removed)
{
	qDebug() << "packagesChanged add:" << added << "rem:" << removed;
	//TODO implement packagesChanged

	auto db = DatabaseController::instance();

	//check if already in db
	foreach (auto pac, removed) {
		auto info = db->getInfo(pac);
		if(info.isValid() && !info.removed){
			info.removed = true;
			_pacInfoList[pac] = info;
		}
	}

	QSet<QString> addedSet;
	foreach(auto pac, added){
		auto info = db->getInfo(pac);
		if(info.isValid()){
			if(info.removed){
				info.removed = false;
				_pacInfoList[pac] = info;
			}
		} else
			addedSet.insert(pac);
	}

	//extra filters
	QHash<QString, QRegularExpression> eCache;
	auto extraFilters = db->extraFilters();
	foreach(auto filter, extraFilters)
		eCache[filter.regex] = createRegEx(filter.regex);

	applyFilters<ExtraFilter>(addedSet, extraFilters,
	[&eCache](QString pac, ExtraFilter filter){
		if(!eCache[filter.regex].isValid())
			return false;
		return eCache[filter.regex].match(pac).hasMatch();
	}, [](ExtraFilter filter){
		return tr("Extra Filter: %1").arg(filter.regex);
	});

	//filters
	QHash<QString, QPair<QRegularExpression, QStringList>> cache;
	auto filters = db->filters().values();
	foreach (auto filter, filters){
		if(filter.plugin == PluginLoader::currentPlugin())
			cache[filter.name] = {createRegEx(filter.regex),
								  PluginLoader::plugin()->listPackages(QVector<bool>::fromList(filter.pluginFilters))};
	}

	applyFilters<FilterInfo>(addedSet, filters,
	[&cache](QString pac, FilterInfo filter){
		if(!cache.contains(filter.name) ||
			(!filter.regex.isEmpty() && !cache[filter.name].first.match(pac).hasMatch()) ||
			!cache[filter.name].first.isValid())
			return false;
		return cache[filter.name].second.contains(pac);
	}, [](FilterInfo filter){
		return tr("Filter: %1").arg(filter.name);
	});

	//global
	switch (db->globalMode()) {
	case FilterInfo::Ask:
		foreach (auto pac, addedSet)
			_uPacInfoList[pac] = {pac, QSysInfo::machineHostName()};
		break;
	case FilterInfo::Add:
		foreach (auto pac, addedSet)
			_pacInfoList[pac] = pac;
		break;
	case FilterInfo::Skip:
		break;
	default:
		Q_UNREACHABLE();
		break;
	}

	emit updateDatabase(_pacInfoList.values(), _uPacInfoList.values());
}

QRegularExpression ChangeFilter::createRegEx(QString pattern)
{
	auto re = QRegularExpression{pattern, QRegularExpression::OptimizeOnFirstUsageOption | QRegularExpression::DontCaptureOption};
	if(!re.isValid()){
		qWarning() << tr("invalid regular expression") << pattern
				   << tr("with error:") << re.errorString();
	}

	return re;
}

template<typename TFilter>
void ChangeFilter::applyFilters(QSet<QString> &packages,
								const QList<TFilter> &filters,
								const std::function<bool(QString, TFilter)> &filterFn,
								const std::function<QString(TFilter)> &filterNameFn)
{
	for(auto it = packages.begin(); it != packages.end();){
		QStringList fmList;

		auto mode = FilterInfo::Ask;
		foreach(auto filter, filters){
			if(filterFn(*it, filter)){
				fmList.append(filterNameFn(filter));
				mode = filter.mode;
			}
		}

		if(fmList.size() > 1){
			_uPacInfoList[*it] = {*it, QSysInfo::machineHostName(), fmList};
			it = packages.erase(it);
		} else if(fmList.size() == 1) {
			switch (mode) {
			case FilterInfo::Ask:
				_uPacInfoList[*it] = {*it, QSysInfo::machineHostName()};
				break;
			case FilterInfo::Add:
				_pacInfoList[*it] = *it;
				break;
			case FilterInfo::Skip:
				break;
			default:
				Q_UNREACHABLE();
				break;
			}
			it = packages.erase(it);
		} else
			it++;
	}
}
