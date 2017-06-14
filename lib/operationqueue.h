#ifndef OPERATIONQUEUE_H
#define OPERATIONQUEUE_H

#include "libpacsync_global.h"
#include <QObject>
#include <QQueue>

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

	explicit OperationQueue(QObject *parent = nullptr);

	OpertionsFlags operations() const;

public slots:
	void addOperations(const QStringList &install, const QStringList &uninstall);

signals:
	void operationsChanged(OpertionsFlags operations);

private:
	QStringList _nextOp;
	OpertionsFlags _opFlags;
};

#endif // OPERATIONQUEUE_H
