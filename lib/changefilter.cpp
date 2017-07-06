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

	//already in db?
	foreach (auto pac, removed) {
		auto info = db->getInfo(pac);
		if(info.isValid() && !info.removed){
			info.removed = true;
			_pacInfoList[pac] = info;
		}
	}

	QSet<QString> addedRest;
	foreach(auto pac, added){
		auto info = db->getInfo(pac);
		if(info.isValid()){
			if(info.removed){
				info.removed = false;
				_pacInfoList[pac] = info;
			}
		} else
			addedRest.insert(pac);
	}

/*	QHash<QString, QRegularExpression> cache;
	auto extraFilters = db->extraFilters();
	foreach(auto filter, extraFilters){
		cache[filter.regex] = {QString(), QRegularExpression::OptimizeOnFirstUsageOption |
							   QRegularExpression::DontCaptureOption};
	}

	auto filterFn = [](QString pac, ExtraFilter filter){

	};
	auto filterNameFn = [](ExtraFilter filter){
		return filter.regex;
	};

	applyFilters<ExtraFilter>(QSet<QString>::fromList(added), extraFilters, filterFn, filterNameFn);

*/
	//extra filters








/*
	foreach (auto filter, db->extraFilters()) {
		if(!setRegexPattern(filter.regex))
			continue;

		foreach(auto pac, added){
			if(_re.match(*it).hasMatch()){
				if(filter.mode == FilterInfo::Ask || _pacInfoList.contains(pac)) {
					_uPacInfoList[pac] = UnclearPackageInfo(pac,
															QSysInfo::machineHostName(),
															tr("Extra Filter: %1").arg(filter.regex));
				} else if(filter.mode == FilterInfo::Add)
					_pacInfoList[pac] = pac;

				addedRest.remove(pac);
			}
		}
	}

	//filters
	erasedPackages.clear();
	foreach(auto filter, db->filters()){
		if(filter.plugin != PluginLoader::currentPlugin())
			continue;

		if(!filter.regex.isEmpty()){
			if(!setRegexPattern(filter.regex))
				continue;
		}


		auto pacListFiltered = PluginLoader::plugin()->listPackages(filter.pluginFilters);
		for(auto it = added.begin(); it != added.end();){
			if((filter.regex.isEmpty() || _re.match(*it).hasMatch()) && pacListFiltered.contains(*it)){

				erasedPackages.append(*it);
				it = added.erase(it);
			} else
				it++;
		}
	}*/

	//global
	for(auto it = added.begin(); it != added.end();){

	}

	emit updateDatabase(_pacInfoList.values());//TODO connect and use
	emit packagesUnclear(_uPacInfoList.values());//TODO connect and use
}

bool ChangeFilter::setRegexPattern(QString pattern)
{
/*	_re.setPattern(pattern);

	if(!_re.isValid()){
		qWarning() << tr("invalid regular expression") << pattern
				   << tr("with error:") << _re.errorString();
		return false;
	}*/
	return true;
}

template<typename TFilter>
void ChangeFilter::applyFilters(const QSet<QString> &packages,
								const QList<TFilter> &filters,
								const std::function<bool(QString, TFilter)> &filterFn,
								const std::function<QString(TFilter)> &filterNameFn)
{
	foreach(auto pac, packages){
		QStringList fmList;

		auto mode = FilterInfo::Ask;
		foreach(auto filter, filters){
			if(filterFn(pac, filter)){
				fmList.append(filterNameFn(filter));
				mode = filter.mode;
			}
		}

		if(fmList.size() > 1)
			_uPacInfoList[pac] = {pac, QSysInfo::machineHostName(), fmList};
		else {
			switch (mode) {
			case FilterInfo::Ask:
				_uPacInfoList[pac] = {pac, QSysInfo::machineHostName()};
				break;
			case FilterInfo::Add:
				_pacInfoList[pac] = pac;
				break;
			default:
				break;
			}
		}
	}
}
