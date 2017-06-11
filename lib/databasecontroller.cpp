#include "databasecontroller.h"

DatabaseController::DatabaseController(QObject *parent) :
	QObject(parent),
	_settings(new QSettings()),
	_dbFile(new QFile())
{
	_settings->beginGroup(QStringLiteral("DatabaseController"));
}

QStringList DatabaseController::listPackages() const
{

}

QString DatabaseController::currentPath() const
{

}

void DatabaseController::createDb(const QString &path, const QStringList &packages)
{
	removeOldDb();

	_settings->setValue(QStringLiteral("path"), path);
	_dbFile->setFileName(path);
}

void DatabaseController::loadDb(const QString &path)
{

}

void DatabaseController::updateDb(const QStringList &packages)
{

}

void DatabaseController::removeOldDb()
{

}
