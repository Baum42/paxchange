#ifndef PACSTATE_H
#define PACSTATE_H

#include <QObject>

class PacState
{
	Q_GADGET

	Q_PROPERTY(QString name MEMBER name)
	Q_PROPERTY(bool installed MEMBER installed)
	Q_PROPERTY(bool baumFilter MEMBER baumFilter)

public:
	PacState();
	QString name;
	bool installed;
	bool baumFilter;
	bool _42Filter;
};

#endif // PACSTATE_H
