#include "changefilter.h"
#include "databasecontroller.h"


ChangeFilter::ChangeFilter(QObject *parent) :
	QObject(parent),
	_re(QString(), QRegularExpression::OptimizeOnFirstUsageOption | QRegularExpression::DontCaptureOption)
{}

void ChangeFilter::packagesChanged(QStringList added, QStringList removed)
{
	qDebug() << "packagesChanged add:" << added << "rem:" << removed;
	//TODO implement packagesChanged

	auto db = DatabaseController::instance();

	auto pacList = db->listPackages();

	//already in db?
	foreach (auto pac, removed) {
		auto info = db->getInfo(pac);
		if(info.isValid() && !info.removed){
			info.removed = true;
			_pacInfoList[pac] = info;
		}
	}
	for(auto it = added.begin(); it != added.end();){
		auto info = db->getInfo(*it);
		if(info.isValid()){
			if(info.removed){
				info.removed = false;
				_pacInfoList[*it] = info;
			}
			it = added.erase(it);
		} else
			it++;
	}

	//extra filters
	foreach (auto filter, db->extraFilters()) {
		if(!setRegexPattern(filter.regex))
			continue;

		for(auto it = added.begin(); it != added.end();){
			if(_re.match(*it).hasMatch()){
				if(filter.mode == FilterInfo::Ask || _pacInfoList.contains(*it)) {
					_uPacInfoList[*it] = UnclearPackageInfo(*it,
															QSysInfo::machineHostName(),
															tr("Extra Filter: %1").arg(filter.regex));
				} else if(filter.mode == FilterInfo::Add)
					_pacInfoList[*it] = *it;

				it = added.erase(it);
			}else
				it++;
		}
	}

	//filters
	erasedPackages.clear();//TODO use
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
	}

	//global
	for(auto it = added.begin(); it != added.end();){

	}

	emit updateDatabase(_pacInfoList.values());//TODO connect and use
	emit packagesUnclear(_uPacInfoList.values());//TODO connect and use
}

bool ChangeFilter::setRegexPattern(QString pattern)
{
	_re.setPattern(pattern);

	if(!_re.isValid()){
		qWarning() << tr("invalid regular expression") << pattern
				   << tr("with error:") << _re.errorString();
		return false;
	}
	return true;
}
