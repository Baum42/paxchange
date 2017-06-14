#ifndef OPERATIONQUEUE_H
#define OPERATIONQUEUE_H

#include "libpacsync_global.h"
#include <QObject>
#include <QQueue>
#include "packagemanagerplugin.h"
class DatabaseController;

class LIBPACSYNC_SHARED_EXPORT OperationQueue : public QObject
{
	Q_OBJECT

	Q_PROPERTY(OpertionsFlags operations READ operations NOTIFY operationsChanged)

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

public slots:
	void setOperations(const QStringList &install, const QStringList &uninstall);
	void startOperation();

signals:
	void operationsChanged(OpertionsFlags operations);

private slots:
	void pluginOpDone();

private:
	QStringList _nextOp;
	OpertionsFlag _nextOpFlag;
	OpertionsFlags _opFlags;
	bool _operating;

	DatabaseController *_controller;
	PackageManagerPlugin *_plugin;
};

#endif // OPERATIONQUEUE_H
