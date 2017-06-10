#include "exception.h"

Exception::Exception(const QString &what) :
	_what(what.toUtf8())
{}

const char *Exception::what() const noexcept
{
	return _what.constData();
}
