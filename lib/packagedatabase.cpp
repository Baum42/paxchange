#include "packagedatabase.h"



PackageInfo::PackageInfo() :
	name(""),
	conflict(false)
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
