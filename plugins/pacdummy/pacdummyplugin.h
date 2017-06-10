#ifndef PACDUMMYPLUGIN_H
#define PACDUMMYPLUGIN_H

#include "pacstate.h"

#include <packagemanagerplugin.h>
#include <QJsonSerializer>

class PacDummyPlugin : public PackageManagerPlugin
{
	Q_OBJECT

	Q_PLUGIN_METADATA(IID PackageManagerPlugin_iid FILE "pacdummy.json")

public:
	PacDummyPlugin(QObject *parent = nullptr);

	QList<FilterInfo> extraFilters() override;
	QStringList listPackages(QList<bool> extraFilters) override;
	void startInstallation(const QStringList &packages, bool noConfirm) override;
	void startUninstallation(const QStringList &packages, bool noConfirm) override;

private:
	QList<PacState> _pacList;
	QJsonSerializer *_js;
};

#endif // PACDUMMYPLUGIN_H
