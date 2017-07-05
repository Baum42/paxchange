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
	return name == other.name;
}

UnclearPackageInfo::UnclearPackageInfo(const PackageInfo &base) :
	PackageInfo(base)
{}

bool UnclearPackageInfo::operator ==(const UnclearPackageInfo &other) const
{
	return ((PackageInfo)*this) == (PackageInfo)other;
}

FilterInfo::FilterInfo(const QString &name, const QString &plugin) :
	name(name),
	plugin(plugin),
	mode(Ask),
	pluginFilters(),
	regex()
{}

bool FilterInfo::operator ==(const FilterInfo &other) const
{
	return name == other.name;
}

ExtraFilter::ExtraFilter(QString regex, FilterInfo::Mode mode) :
	regex(regex),
	mode(mode)
{}

bool ExtraFilter::operator ==(const ExtraFilter &other) const
{
	return regex == other.regex;
}

PackageDatabase::PackageDatabase() :
	packages(),
	unclearPackages(),
	globalMode(FilterInfo::Ask),
	filters(),
	extraFilters(),
	settings()
{}
