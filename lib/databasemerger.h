#ifndef DATABASEMERGER_H
#define DATABASEMERGER_H

#include <QJsonSerializer>
#include <QObject>

class DatabaseMerger : public QObject
{
	Q_OBJECT

public:
	explicit DatabaseMerger(QObject *parent = nullptr);

	bool mergeDb(QString dbPath);

	QList<QPair<QString, QString>> log() const;

private:
	QJsonSerializer *_js;

	QList<QPair<QString, QString>> _log;//what, package name
};

#endif // DATABASEMERGER_H
