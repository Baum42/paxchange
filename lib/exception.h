#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "libpacsync_global.h"
#include <QString>
#include <QException>

class LIBPACSYNC_SHARED_EXPORT ExceptionBase : public QException
{
public:
	ExceptionBase(const QString &what = QString());

	const char *what() const noexcept override;

protected:
	QByteArray _what;
};

template <typename T>
class Exception : public ExceptionBase
{
public:
	inline Exception(const QString &what = QString());

	void raise() const override;
	QException *clone() const override;
};

#define DEF_EXC(Type) \
	class LIBPACSYNC_SHARED_EXPORT Type : public Exception<Type> { \
	public: \
		inline Type(const QString &what) : \
			Exception(what) \
		{} \
	};

template<typename T>
inline Exception<T>::Exception(const QString &what) :
	ExceptionBase(what)
{}

template<typename T>
void Exception<T>::raise() const
{
	throw *((const T*)this);
}

template<typename T>
QException *Exception<T>::clone() const
{
	auto e = new T(QString());
	e->_what = _what;
	return e;
}

#endif // EXCEPTION_H
