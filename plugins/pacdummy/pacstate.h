#ifndef PACSTATE_H
#define PACSTATE_H

#include <QObject>

class PacState
{
	Q_GADGET

	Q_PROPERTY(QString name MEMBER name)
	Q_PROPERTY(bool installed MEMBER installed)
	Q_PROPERTY(bool filterBaum MEMBER filterBaum)
	Q_PROPERTY(bool filter42 MEMBER filter42)

public:
	PacState();
	QString name;
	bool installed;
	bool filterBaum;
	bool filter42;
};

#endif // PACSTATE_H
