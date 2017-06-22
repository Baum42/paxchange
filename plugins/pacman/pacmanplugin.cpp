#include "pacmanplugin.h"

#include <QDebug>
#include <QProcess>

PacmanPlugin::PacmanPlugin(QObject *parent) :
	PackageManagerPlugin(parent)
{

}


void PacmanPlugin::initialize()
{
}

QList<PacmanPlugin::FilterInfo> PacmanPlugin::extraFilters()
{
	QList<PacmanPlugin::FilterInfo> list;
	list.append({QStringLiteral("&Explicitly installed"), QStringLiteral("Only explicitly installed packages"), false});
	list.append({QStringLiteral("&Leaf packages"), QStringLiteral("Only leaf packages"), false});
	list.append({QStringLiteral("&Foreign packages only"), QStringLiteral("Only packages that are not present in any mirror (includes AUR packages)"), false});
	list.append({QStringLiteral("&Native packages only"), QStringLiteral("Only packages that are present in any mirror (excludes AUR packages)"), false});
	return list;
}

QStringList PacmanPlugin::listAllPackages()
{
	return  listPackages({false, false, false, false});
}

QStringList PacmanPlugin::listPackages(QVector<bool> extraFilters)
{
	if(extraFilters[2] && extraFilters[3])
		return {};

	auto queryString = QStringLiteral("-Qq");

	if(extraFilters[0])
		queryString += QLatin1Char('e');
	if(extraFilters[1])
		queryString += QStringLiteral("tt");
	if(extraFilters[2])
		queryString += QLatin1Char('m');
	if(extraFilters[3])
		queryString += QLatin1Char('n');

	QProcess p;
	p.start(QStringLiteral("pacman"), {queryString});
	if(!p.waitForFinished(5000))
		return {};

	auto list = QString::fromUtf8(p.readAll()).split(QStringLiteral("\n"), QString::SkipEmptyParts);
	qDebug() << list;
	return list;
}

QString PacmanPlugin::installationCmd(const QStringList &packages)
{
}

QString PacmanPlugin::uninstallationCmd(const QStringList &packages)
{
}

bool PacmanPlugin::startGuiInstall(const QStringList &packages)
{
}

bool PacmanPlugin::startGuiUninstall(const QStringList &packages)
{
}

QList<PackageManagerPlugin::SettingsInfo> PacmanPlugin::listSettings()
{
}
