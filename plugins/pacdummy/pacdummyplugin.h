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

	void initialize() override;

	QList<FilterInfo> extraFilters() override;
	QStringList listAllPackages() override;
	QStringList listPackages(QVector<bool> extraFilters) override;
	QString installationCmd(const QStringList &packages) override;
	QString uninstallationCmd(const QStringList &packages) override;

	QList<SettingsInfo> listSettings() const override;

private:
	QList<PacState> _pacList;
	QJsonSerializer *_js;
	QFile *_file;
};

#endif // PACDUMMYPLUGIN_H
