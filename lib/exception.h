#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <QString>
#include <QException>

class Exception : public QException
{
public:
	Exception(const QString &what = QString());

	const char *what() const noexcept override;

protected:
	QByteArray _what;
};

#endif // EXCEPTION_H
