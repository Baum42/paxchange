#include "packagemodel.h"

#include <QColor>
#include <QIcon>
#include <databasecontroller.h>

PackageModel::PackageModel(QObject *parent) :
	QStringListModel(parent)
{}

QVariant PackageModel::data(const QModelIndex &index, int role) const
{
	switch (role) {
	case Qt::DecorationRole:
		return info(index).removed ?
					QVariant(QIcon::fromTheme(QStringLiteral("package-remove"))) :
					QVariant(QIcon::fromTheme(QStringLiteral("package-available")));
	case Qt::ToolTipRole:
		return info(index).name;
	default:
		return QStringListModel::data(index, role);
	}
}

PackageInfo PackageModel::info(const QModelIndex &name) const
{
	return info(name.data().toString());
}

PackageInfo PackageModel::info(const QString &name) const
{
	auto &info = _infoHash[name];
	if(!info.isValid())
		info = DatabaseController::instance()->getInfo(name);
	return info;
}
