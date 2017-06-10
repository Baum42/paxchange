#ifndef PACKARCHPLUGIN_H
#define PACKARCHPLUGIN_H

#include <QGenericPlugin>


class PackArchPlugin : public QGenericPlugin
{
	Q_OBJECT
#if QT_VERSION >= 0x050000
	Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QGenericPluginFactoryInterface" FILE "pacarch.json")
#endif // QT_VERSION >= 0x050000

public:
	PackArchPlugin(QObject *parent = 0);
};

#endif // PACKARCHPLUGIN_H
