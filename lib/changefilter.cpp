#include "changefilter.h"

ChangeFilter::ChangeFilter(QObject *parent) :
	QObject(parent)
{}

void ChangeFilter::packagesChanged(const QStringList &added, const QStringList &removed)
{
	qDebug() << "packagesChanged add:" << added << "rem:" << removed;
	//TODO implement packagesChanged
}
