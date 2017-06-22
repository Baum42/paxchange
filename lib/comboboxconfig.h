#ifndef COMBOBOXCONFIG_H
#define COMBOBOXCONFIG_H

#include "libpacsync_global.h"
#include <QStringList>
#include <QVariant>

class LIBPACSYNC_SHARED_EXPORT ComboboxConfig
{
public:
	QStringList displayNames;
	QList<QVariant> values;
	QVariant defaultValue;
	bool editable;

	ComboboxConfig(QStringList displayNames = {},
				   QList<QVariant> values = {},
				   QVariant defaultValue = {},
				   bool editable = false);

	QVariant getValue(const QString &key) const;
	QString getKey(const QVariant &value) const;
};

Q_DECLARE_METATYPE(ComboboxConfig)

#endif // COMBOBOXCONFIG_H
