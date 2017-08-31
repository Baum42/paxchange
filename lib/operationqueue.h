#ifndef OPERATIONQUEUE_H
#define OPERATIONQUEUE_H

#include "libpaxchange_global.h"
#include <QObject>
#include <QQueue>
#include "packagemanagerplugin.h"
class DatabaseController;

class LIBPAXCHANGE_SHARED_EXPORT OperationQueue : public QObject
{
	Q_OBJECT

	Q_PROPERTY(OpertionsFlags operations READ operations NOTIFY operationsChanged)
	Q_PROPERTY(bool operating READ isOperating NOTIFY operatingChanged)

public:
	enum OpertionsFlag {
		None = 0x00,
		Install = 0x01,
		Uninstall = 0x02,
		Both = (Install | Uninstall)
	};
	Q_DECLARE_FLAGS(OpertionsFlags, OpertionsFlag)
	Q_FLAG(OpertionsFlags)

	explicit OperationQueue(DatabaseController *parent = nullptr);

	OpertionsFlags operations() const;
	OpertionsFlag nextOperation() const;
	bool isOperating() const;

public slots:
	void setOperations(const QStringList &install, const QStringList &uninstall);
	void startOperation();
	void cmdDone();

signals:
	void operationsChanged(OpertionsFlags operations);
	void operatingChanged(bool operating);
	void startCmd(QString cmd);

private:
	QStringList _nextOp;
	OpertionsFlag _nextOpFlag;
	OpertionsFlags _opFlags;
	bool _operating;

	DatabaseController *_controller;
	PackageManagerPlugin *_plugin;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(OperationQueue::OpertionsFlags)

#endif // OPERATIONQUEUE_H
