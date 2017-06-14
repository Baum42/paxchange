#include "databasecontroller.h"
#include "dbsettings.h"
#include <QDir>
#include <QDebug>

static QSettings::Format _format = QSettings::InvalidFormat;

static bool readDbSettings(QIODevice &device, QSettings::SettingsMap &map);
static bool writeDbSettings(QIODevice &device, const QSettings::SettingsMap &map);

QSettings *DbSettings::create(QObject *parent)
{
	if(_format == QSettings::InvalidFormat)
		throw SettingsException(QStringLiteral("Failed to create psdummy settings format"));
	return new QSettings(QDir::temp().absoluteFilePath(QStringLiteral("pacsync-settings.psdummy")), _format, parent);
}

QSharedPointer<QSettings> DbSettings::create()
{
	return QSharedPointer<QSettings>(create(nullptr));
}


void DbSettings::registerSettings()
{
	_format = QSettings::registerFormat(QStringLiteral("psdummy"),
										readDbSettings,
										writeDbSettings);
	if(_format == QSettings::InvalidFormat)
		throw SettingsException(QStringLiteral("Failed to create psdummy settings format"));
}

QString DbSettings::getPath()
{
	return DbSettings::create()->fileName();
}

void DbSettings::triggerChange()
{
	QFile dummyFile(getPath());
	if(!dummyFile.open(QIODevice::WriteOnly))
		qWarning() << "Failed to write dummy with error:" << dummyFile.errorString();
	else {
		dummyFile.write(QByteArray::number(qrand()));
		dummyFile.close();
	}
}

static bool readDbSettings(QIODevice &device, QSettings::SettingsMap &map)
{
	map = DatabaseController::instance()->readSettings();
	return true;
}

static bool writeDbSettings(QIODevice &device, const QSettings::SettingsMap &map)
{
	DatabaseController::instance()->writeSettings(map);
	return true;
}
