#ifndef PACDUMMYPLUGIN_H
#define PACDUMMYPLUGIN_H

#include "pacstate.h"

#include <packagemanagerplugin.h>
#include <QJsonSerializer>
#include <QProcess>
#include <QFile>

class PacDummyPlugin : public PackageManagerPlugin
{
	Q_OBJECT

	Q_PLUGIN_METADATA(IID PackageManagerPlugin_iid FILE "pacdummy.json")

public:
	PacDummyPlugin(QObject *parent = nullptr);

	QList<FilterInfo> extraFilters() override;
	QStringList listAllPackages() override;
	QStringList listPackages(QList<bool> extraFilters) override;
	QString installationCmd(const QStringList &packages) override;
	QString uninstallationCmd(const QStringList &packages) override;

	QList<SettingsInfo> listSettings() override;

private:
	QList<PacState> _pacList;
	QJsonSerializer *_js;
	QProcess *_process;
	QFile *_file;
	QSettings *_settings;
};

#endif // PACDUMMYPLUGIN_H
