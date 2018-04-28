#include "databasecontroller.h"
#include "databasemerger.h"
#include "packagedatabase.h"
#include <QJsonSerializer>

#include <QFile>

DatabaseMerger::DatabaseMerger(QObject *parent) :
	QObject(parent),
	_js(new QJsonSerializer(this)),
	_log()
{}

bool DatabaseMerger::mergeDb(QString dbPath)
{
	try {
		_log.clear();

		QFile currentFile(DatabaseController::instance()->currentPath());
		QFile otherFile(dbPath);
		if(!currentFile.open(QIODevice::ReadWrite))
			throw DatabaseException(currentFile.errorString());
		if(!otherFile.open(QIODevice::ReadOnly))
			throw DatabaseException(otherFile.errorString());

		auto pacDbCurrent = _js->deserializeFrom<PackageDatabase>(&currentFile);
		auto pacDbOther = _js->deserializeFrom<PackageDatabase>(&otherFile);
		otherFile.close();

		//packages: keep install on conflict
		for (auto pacInfo : pacDbOther.packages) {
			if(!pacDbCurrent.packages.contains(pacInfo.name))
				pacDbCurrent.packages[pacInfo.name] = pacInfo;
			else if(pacInfo.removed != pacDbCurrent.packages[pacInfo.name].removed) {
				pacDbCurrent.packages[pacInfo.name].removed = false;
				_log.append(tr("Conflict on install state of package %1. Has been set to \"install\"").arg(pacInfo.name));
			}
		}

		//unlearPackages: keep current on conflict
		for (auto unclearPac : pacDbOther.unclearPackages) {
			if(!pacDbCurrent.unclearPackages.contains(unclearPac.name))
				pacDbCurrent.unclearPackages[unclearPac.name] = unclearPac;
			else if(!unclearPac.equals(pacDbCurrent.unclearPackages[unclearPac.name]))
				_log.append(tr("Conflict on reason for unclear package %1 - kept info of current database").arg(unclearPac.name));
		}


		//mode: keep current on conflict
		if(pacDbOther.globalMode != pacDbCurrent.globalMode)
			_log.append(tr("Conflict on global filter mode - kept state of current database"));

		//filter: auto rename on conflict
		for (auto filter : pacDbOther.filters){
			if(!pacDbCurrent.filters.contains(filter.name))
				pacDbCurrent.filters[filter.name] = filter;
			else if(!filter.equals(pacDbCurrent.filters[filter.name])){
				const auto oldName = filter.name;
				auto idx = 0;
				do
					filter.name = tr("%1-%2").arg(oldName).arg(++idx);
				while(pacDbCurrent.filters.contains(filter.name));
				pacDbCurrent.filters[filter.name] = filter;
				_log.append(tr("Conflict on filter %1 - renamed new filter to %2")
							.arg(oldName)
							.arg(filter.name));
			}
		}

		//extra filter: keep current on conflict
		for (auto eFilter : pacDbOther.extraFilters) {
			auto index = pacDbCurrent.extraFilters.indexOf(eFilter);
			if(index == -1)
				pacDbCurrent.extraFilters.append(eFilter);
			else if(!eFilter.equals(pacDbCurrent.extraFilters.at(index)))
				_log.append(tr("Conflict on extra filter %1 - kept state of current database").arg(eFilter.regex));
		}

		//settings keep current and log
		if(pacDbCurrent.settings != pacDbOther.settings)
			_log.append(tr("Unable to merge settings - kept state of current database"));

		currentFile.seek(0);
		_js->serializeTo<PackageDatabase>(&currentFile, pacDbCurrent);
		currentFile.close();
		return true;
	} catch(QException &e) {
		qWarning() << "Failed to merge with error" << e.what();
		return false;
	}
}

QStringList DatabaseMerger::log() const
{
	return _log;
}
