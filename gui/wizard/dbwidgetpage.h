#ifndef DBWIDGETPAGE_H
#define DBWIDGETPAGE_H

#include <QWizardPage>
#include <QMetaProperty>
#include <QVBoxLayout>
template <typename T>
class DbWidgetPage : public QWizardPage
{
public:
	explicit DbWidgetPage(const QString &fieldName, QWidget *parent = nullptr);

	void initializePage() override;
	void cleanupPage() override;

protected:
	T *_widget;
};

template <typename T>
DbWidgetPage<T>::DbWidgetPage(const QString &fieldName, QWidget *parent) :
	QWizardPage(parent),
	_widget(new T(this))
{
	setLayout(new QVBoxLayout(this));
	layout()->addWidget(_widget);

	setTitle(_widget->windowTitle());
	setSubTitle(_widget->whatsThis());

	registerField(fieldName, _widget, _widget->metaObject()->userProperty().name());
}

template <typename T>
void DbWidgetPage<T>::initializePage()
{
	setFinalPage(field(QStringLiteral("isLoad")).toBool());
}

template <typename T>
void DbWidgetPage<T>::cleanupPage()
{
	_widget->metaObject()->userProperty().reset(_widget);
}


#endif // DBWIDGETPAGE_H
