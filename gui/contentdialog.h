#ifndef CONTENTDIALOG_H
#define CONTENTDIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QVariant>

class ContentDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ContentDialog(QWidget *parent = nullptr);

	static QVariant execute(QWidget *contentWidget,
							const QVariant &defaultValue = QVariant(),
							QWidget *parent = nullptr);
	template<typename TWidget, typename TData = QVariant>
	static TData execute(const TData &defaultValue = TData(), QWidget *parent = nullptr, bool *ok = nullptr);

	static QVariantList execute(const QString &windowTitle,
								const QList<QWidget*> &contentWidgets,
								const QList<QVariant> &defaultValues = {},
								QWidget *parent = nullptr);

private:
	QTabWidget *tabWidget;
};

template<typename TWidget, typename TData>
TData ContentDialog::execute(const TData &defaultValue, QWidget *parent, bool *ok)
{
	auto v = execute(new TWidget(nullptr), QVariant::fromValue(defaultValue), parent);
	if(ok)
		*ok = v.isValid();
	if(v.isValid())
		return v.template value<TData>();
	else
		return defaultValue;
}

#endif // CONTENTDIALOG_H
