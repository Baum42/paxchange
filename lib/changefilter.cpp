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
	for(auto it = added.begin(); it != added.end();){
		auto info = db->getInfo(*it);
		if(info.isValid()){
			if(info.removed){
				info.removed = false;
				_pacInfoList.append(info);
			}
			it = added.erase(it);
		} else
			it++;
	}
	foreach (auto pac, removed) {
		auto info = db->getInfo(pac);
		if(info.isValid() && !info.removed){
			info.removed = true;
			_pacInfoList.append(info);
		}
	}

	//extra filters
	foreach (auto filter, db->extraFilters()) {
		_re.setPattern(filter.regex);

		if(!_re.isValid()){
			qWarning() << tr("invalid regular expression") << filter.regex
					   << tr("with error:") << _re.errorString();
			continue;
		}

		//install
		appendExtraFilter(added, filter);//TODO remove function if not duplicated code
	}

	//filters
	foreach(auto filter, db->filters()){
		//install
		for(auto it = added.begin(); it != added.end();){

		}
	}

	//global
	//install
	for(auto it = added.begin(); it != added.end();){

	}

	emit updateDatabase(_pacInfoList);//TODO connect and use
	emit packagesUnclear(_uPacInfoList);//TODO connect and use
}

void ChangeFilter::appendExtraFilter(QStringList &list, ExtraFilter filter)
{
	for(auto it = list.begin(); it != list.end();){
		if(_re.match(*it).hasMatch()){
			if(filter.mode == FilterInfo::Ask) {
				_uPacInfoList.append(UnclearPackageInfo(*it,
														QSysInfo::machineHostName(),
														tr("Extra Filter: %1").arg(filter.regex)));
			} else if(filter.mode == FilterInfo::Add)
				_pacInfoList.append(*it);

			it = list.erase(it);
		}else
			it++;
	}
}
