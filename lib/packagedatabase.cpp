#include "packagedatabase.h"

PackageInfo::PackageInfo(QString name, bool removed) :
	name(name),
	removed(removed)
{}

bool PackageInfo::isValid() const
{
	return !name.isNull();
}

bool PackageInfo::operator ==(const PackageInfo &other) const
{
	return this->name == other.name;
}

PackageDatabase::PackageDatabase() :
	mode(BlackList),
	packages()
{}
