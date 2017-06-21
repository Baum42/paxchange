#include "pacdummyplugin.h"

#include <QCoreApplication>
#include <QTemporaryFile>


PacDummyPlugin::PacDummyPlugin(QObject *parent) :
	PackageManagerPlugin(parent),
	_js(new QJsonSerializer(this)),
	_file(new QFile(QCoreApplication::applicationDirPath() + QStringLiteral("/fakeman.json"), this)),
	_settings(nullptr)
{
	QJsonSerializer::registerListConverters<PacState>();
}

void PacDummyPlugin::initialize()
{
	_settings = createSyncedSettings(this);

	if(_settings->value(QStringLiteral("delFakeman")).toBool())
		_file->remove();

	if(QFile::copy(QStringLiteral(":/fakeman.json"), _file->fileName()))
		_file->setPermissions(QFile::ReadOwner | QFile::WriteOwner);

	_file->open(QIODevice::ReadOnly);
	_pacList = _js->deserializeFrom<QList<PacState>>(_file);
	_file->close();
}

QList<PacDummyPlugin::FilterInfo> PacDummyPlugin::extraFilters()
{
	QList<PacDummyPlugin::FilterInfo> list;
	list.append({QStringLiteral("&baum"), QStringLiteral("baum is important"), true});
	list.append({QStringLiteral("42"), QString(), false});
	return list;
}

QStringList PacDummyPlugin::listAllPackages()
{
	QStringList list;
	foreach (auto pacState, _pacList) {
		if(!pacState.installed)
			continue;
		list.append(pacState.name);
	}

	return list;
}

QStringList PacDummyPlugin::listPackages(QVector<bool> extraFilters)
{
	QStringList list;
	foreach (auto pacState, _pacList) {
		if(!pacState.installed)
			continue;

		if(extraFilters[0] && !pacState.filterBaum)
			continue;

		if(extraFilters[1] && !pacState.filter42)
			continue;

		list.append(pacState.name);
	}
	return list;
}

QString PacDummyPlugin::installationCmd(const QStringList &packages)
{
	QTemporaryFile tmp;
	tmp.setAutoRemove(false);
	tmp.open();

	QTextStream stream(&tmp);
	stream << QStringLiteral("#!/bin/sh\n");

	if(_settings->value(QStringLiteral("secret")).toInt() == 42)
		stream << QStringLiteral("echo Baum!\n");

	stream << QStringLiteral("echo installing the following packages:\n")
		   << QStringLiteral("echo ") << packages.join(QStringLiteral(" ")) << QStringLiteral("\n")
		   << QStringLiteral("read -p \"Press enter to continue\"\n");

	stream.flush();
	tmp.close();
	tmp.setPermissions(tmp.permissions() | QFileDevice::ExeUser);

	bool stateChanged = false;
	foreach (auto package, packages) {
		for(int i = 0; i < _pacList.size(); i++){
			if(_pacList[i].name == package){
				_pacList[i].installed = true;
				stateChanged = true;
			}
		}
	}

	if(stateChanged){
		_file->open(QIODevice::WriteOnly);
		_js->serializeTo<QList<PacState>>(_file, _pacList);
		_file->close();
	}

	return tmp.fileName();
}

QString PacDummyPlugin::uninstallationCmd(const QStringList &packages)
{
	QTemporaryFile tmp;
	tmp.setAutoRemove(false);
	tmp.open();

	QTextStream stream(&tmp);
	stream << QStringLiteral("#!/bin/sh\n");

	if(_settings->value(QStringLiteral("secret")).toInt() == 42)
		stream << QStringLiteral("echo Baum!\n");

	stream << QStringLiteral("echo uninstalling the following packages:\n")
		   << QStringLiteral("echo ") << packages.join(QStringLiteral(" ")) << QStringLiteral("\n")
		   << QStringLiteral("read -p \"Press enter to continue\"\n");

	stream.flush();
	tmp.close();
	tmp.setPermissions(tmp.permissions() | QFileDevice::ExeUser);

	bool stateChanged = false;
	foreach (auto package, packages) {
		for(int i = 0; i < _pacList.size(); i++){
			if(_pacList[i].name == package){
				_pacList[i].installed = false;
				stateChanged = true;
			}
		}
	}

	if(stateChanged){
		_file->open(QIODevice::WriteOnly);
		_js->serializeTo<QList<PacState>>(_file, _pacList);
		_file->close();
	}

	return tmp.fileName();
}

QList<PackageManagerPlugin::SettingsInfo> PacDummyPlugin::listSettings()
{
	return {
		{
			tr("Delete fakeman.json"),
			tr("Delete the fake installed package database on every program start"),
			QStringLiteral("delFakeman"),
			QMetaType::Bool,
			false
		},
		{
			tr("Baum"),
			tr("Psst - the secret is the meaning of life"),
			QStringLiteral("secret"),
			QMetaType::Int
		}
	};
}
