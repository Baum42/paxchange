#include "pacmanplugin.h"

#include <QDebug>
#include <QFile>
#include <QProcess>
#include <QStandardPaths>
#include <comboboxconfig.h>
#include <databasecontroller.h>

PacmanPlugin::PacmanPlugin(QObject *parent) :
	PackageManagerPlugin(parent)
{}

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
	if(!p.waitForFinished(5000)) {
		qCritical() << "Package read failed" << p.errorString();
		DatabaseController::instance()->guiError(tr("Failed to load packages from pacman!"), true);
		return {};
	}

	return QString::fromUtf8(p.readAll()).split(QStringLiteral("\n"), QString::SkipEmptyParts);
}

QString PacmanPlugin::installationCmd(const QStringList &packages)
{
	return createCmd(QStringLiteral("instparams"), packages);
}

QString PacmanPlugin::uninstallationCmd(const QStringList &packages)
{
	return createCmd(QStringLiteral("uninstparams"), packages);
}

QList<PackageManagerPlugin::SettingsInfo> PacmanPlugin::listSettings() const
{
	QList<QPair<QString, bool>> frontends = {
		{QStringLiteral("pacaur"), false},
		{QStringLiteral("yaourt"), false},
		{QStringLiteral("pacman"), true}
	};
	auto index = 0;
	while(index < (frontends.size() - 1) &&
		  QStandardPaths::findExecutable(frontends[index].first).isNull())
		index++;

	return {
		{
			tr("Pacman &frontend"),
			tr("Select a pacman frontend to be used for un/installation"),
			QStringLiteral("frontend"),
			qMetaTypeId<ComboboxConfig>(),
			QVariant::fromValue<ComboboxConfig>({
				{QStringLiteral("pacaur"), QStringLiteral("yaourt"), QStringLiteral("pacman")},
				{},
				frontends[index].first,
				true
			})
		},
		{
			tr("Requires &root"),
			tr("Specify if the frontend of your choice needs to be run as root"),
			QStringLiteral("sudo"),
			QMetaType::Bool,
			frontends[index].second
		},
		{
			tr("&Install parameters"),
			tr("The parameters to be used for installation. "
			   "%p is replaced by the list of packages (space seperated). "
			   "If missing, the packages are appended to the command line"),
			QStringLiteral("instparams"),
			QMetaType::QString,
			QStringLiteral("-S %p")
		},
		{
			tr("&Uninstall parameters"),
			tr("The parameters to be used for uninstallation. "
			   "%p is replaced by the list of packages (space seperated). "
			   "If missing, the packages are appended to the command line"),
			QStringLiteral("uninstparams"),
			QMetaType::QString,
			QStringLiteral("-R %p")
		}
	};
}

void PacmanPlugin::forwardedArguments(QStringList args)
{
	if(args.size() < 2){
		qWarning() << tr("Invalid plugin parameters");
		return;
	}

	auto first = args.takeFirst();
	if(first == QStringLiteral("i"))
		emit packagesChanged(args, {});
	else if(first == QStringLiteral("r"))
		emit packagesChanged({}, args);
	else
		qWarning() << tr("Invalid plugin parameters");
}

QString PacmanPlugin::createCmd(QString key, QStringList packages)
{
	SyncedSettings settings;

	QString cmd;
	if(settings.value(QStringLiteral("sudo")).toBool())
		cmd = QStringLiteral("sudo ");

	cmd += QStandardPaths::findExecutable(settings.value(QStringLiteral("frontend")).toString())
		+ QStringLiteral(" ") + settings.value(key).toString();

	if(cmd.contains(QStringLiteral("%p")))
		cmd.replace(QStringLiteral("%p"), packages.join(QStringLiteral(" ")));
	else
		cmd += QStringLiteral(" ") + packages.join(QStringLiteral(" "));

	return cmd;
}
