#include "packagedatabase.h"




PackageInfo::PackageInfo(QString name, bool conflict) :
	name(name),
	conflict(conflict)
{

}

bool PackageInfo::operator ==(const PackageInfo &other) const
{
	return this->name == other.name;
}

PackageDatabase::PackageDatabase() :
	mode(BlackList),
	packages()
{

}
