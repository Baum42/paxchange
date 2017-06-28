#ifndef SYNCEDSETTINGS_H
#define SYNCEDSETTINGS_H

#include <QVariant>

class SyncedSettings
{
public:
	SyncedSettings();
	~SyncedSettings();

	QVariant value(const QString &key, const QVariant &overwriteDefault = QVariant()) const;
	void setValue(const QString &key, const QVariant &value);
	void remove(const QString &key);

	void beginGroup(const QString &key);
	void endGroup();

	static QString pluginKey(const QString &key);

private:
	QStringList _groups;
	QVariantHash _changes;

	QString realKey(const QString &key) const;
	QVariant defaultValue(const QString &localKey) const;
};

#endif // SYNCEDSETTINGS_H
