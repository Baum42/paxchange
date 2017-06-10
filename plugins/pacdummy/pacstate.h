#ifndef PACSTATE_H
#define PACSTATE_H

#include <QObject>

class PacState
{
	Q_GADGET

	Q_PROPERTY(QString name MEMBER name)
	Q_PROPERTY(bool installed MEMBER installed)

public:
	PacState();
	QString name;
	bool installed;
};

#endif // PACSTATE_H
