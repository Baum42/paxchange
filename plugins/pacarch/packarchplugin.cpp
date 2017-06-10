#include "packarchplugin.h"


PackArchPlugin::PackArchPlugin(QObject *parent) :
	QGenericPlugin(parent)
{
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(pacarch, PackArchPlugin)
#endif // QT_VERSION < 0x050000
