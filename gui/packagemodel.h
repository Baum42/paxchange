#ifndef PACKAGEMODEL_H
#define PACKAGEMODEL_H

#include <QStringListModel>
#include <packagedatabase.h>

class PackageModel : public QStringListModel
{
	Q_OBJECT

public:
	explicit PackageModel(QObject *parent = nullptr);

	QVariant data(const QModelIndex &index, int role) const override;

private:
	mutable QHash<QString, PackageInfo> _infoHash;

	PackageInfo info(const QModelIndex &name) const;
	PackageInfo info(const QString &name) const;
};

#endif // PACKAGEMODEL_H
