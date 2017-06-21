#ifndef PACMAN_H
#define PACMAN_H

#include <packagemanagerplugin.h>

class Pacman : public PackageManagerPlugin
{
	Q_OBJECT

	Q_PLUGIN_METADATA(IID PackageManagerPlugin_iid FILE "pacman.json")

public:
	Pacman(QObject *parent = nullptr);

public:
	void initialize() override;
	QList<FilterInfo> extraFilters() override;
	QStringList listAllPackages() override;
	QStringList listPackages(QVector<bool> extraFilters) override;
	QString installationCmd(const QStringList &packages) override;
	QString uninstallationCmd(const QStringList &packages) override;
	bool startGuiInstall(const QStringList &packages) override;
	bool startGuiUninstall(const QStringList &packages) override;
	QList<SettingsInfo> listSettings() override;
};

#endif // PACMAN_H
