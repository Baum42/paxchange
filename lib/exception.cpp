#include "exception.h"

ExceptionBase::ExceptionBase(const QString &what) :
	_what(what.toUtf8())
{}

const char *ExceptionBase::what() const noexcept
{
	return _what.constData();
}
