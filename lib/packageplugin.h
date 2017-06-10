#ifndef PACKAGEPLUGIN_H
#define PACKAGEPLUGIN_H

#include <QObject>

class PackagePlugin : public QObject
{
	Q_OBJECT
public:
	explicit PackagePlugin(QObject *parent = nullptr);

signals:

public slots:
};

#endif // PACKAGEPLUGIN_H