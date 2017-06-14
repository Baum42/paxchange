#ifndef DBSETTINGS_H
#define DBSETTINGS_H

#include "libpacsync_global.h"
#include <QSettings>
#include <QSharedPointer>
#include "exception.h"

DEF_EXC(SettingsException)

namespace DbSettings
{
//plugin/public
LIBPACSYNC_SHARED_EXPORT QSettings *create(QObject *parent);
LIBPACSYNC_SHARED_EXPORT QSharedPointer<QSettings> create();

//internal
LIBPACSYNC_SHARED_EXPORT void registerSettings();
LIBPACSYNC_SHARED_EXPORT QString getPath();
LIBPACSYNC_SHARED_EXPORT void triggerChange();
}

#endif // DBSETTINGS_H
