#ifndef DATABASEMERGER_H
#define DATABASEMERGER_H

#include "libpaxchange_global.h"
#include <QObject>
class QJsonSerializer;

class LIBPAXCHANGE_SHARED_EXPORT DatabaseMerger : public QObject
{
	Q_OBJECT

public:
	explicit DatabaseMerger(QObject *parent = nullptr);

	bool mergeDb(QString dbPath);

	QStringList log() const;

private:
	QJsonSerializer *_js;

	QStringList _log;//what, package name
};

#endif // DATABASEMERGER_H
