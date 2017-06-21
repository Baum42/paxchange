#include "pacman.h"

Pacman::Pacman(QObject *parent) :
	PackageManagerPlugin(parent)
{

}


void Pacman::initialize()
{
}

QList<PackageManagerPlugin::FilterInfo> Pacman::extraFilters()
{
}

QStringList Pacman::listAllPackages()
{
}

QStringList Pacman::listPackages(QVector<bool> extraFilters)
{
}

QString Pacman::installationCmd(const QStringList &packages)
{
}

QString Pacman::uninstallationCmd(const QStringList &packages)
{
}

bool Pacman::startGuiInstall(const QStringList &packages)
{
}

bool Pacman::startGuiUninstall(const QStringList &packages)
{
}

QList<PackageManagerPlugin::SettingsInfo> Pacman::listSettings()
{
}
