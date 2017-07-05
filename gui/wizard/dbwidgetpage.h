#ifndef DBWIDGETPAGE_H
#define DBWIDGETPAGE_H

#include <QWizardPage>
#include <QMetaProperty>
#include <QVBoxLayout>
#include <databasecontroller.h>

template <typename T>
class DbWidgetPage : public QWizardPage
{
public:
	explicit DbWidgetPage(const QString &fieldName,
						  const QByteArray &dbProperty,
						  QWidget *parent = nullptr);

	void initializePage() override;
	void cleanupPage() override;

	void writeDatabase();

protected:
	T *widget;

private:
	QByteArray _dbProperty;
};

template <typename T>
DbWidgetPage<T>::DbWidgetPage(const QString &fieldName, const QByteArray &dbProperty, QWidget *parent) :
	QWizardPage(parent),
	widget(new T(this)),
	_dbProperty(dbProperty)
{
	setLayout(new QVBoxLayout(this));
	layout()->addWidget(widget);

	setTitle(widget->windowTitle());
	setSubTitle(widget->whatsThis());

	registerField(fieldName, widget, widget->metaObject()->userProperty().name());
}

template <typename T>
void DbWidgetPage<T>::initializePage()
{
	if(field(QStringLiteral("isLoad")).toBool()) {
		setFinalPage(true);

		if(!_dbProperty.isNull()) {
			auto inst = DatabaseController::instance();
			auto index = inst->metaObject()->indexOfProperty(_dbProperty.constData());
			if(index != -1) {
				auto prop = inst->metaObject()->property(index);
				auto userProp = widget->metaObject()->userProperty();
				userProp.write(widget, prop.read(inst));
			}
		}
	} else
		setFinalPage(false);
}

template <typename T>
void DbWidgetPage<T>::cleanupPage()
{
	widget->metaObject()->userProperty().reset(widget);
}

template<typename T>
void DbWidgetPage<T>::writeDatabase()
{
	if(!_dbProperty.isNull()) {
		auto inst = DatabaseController::instance();
		auto index = inst->metaObject()->indexOfProperty(_dbProperty.constData());
		if(index != -1) {
			auto prop = inst->metaObject()->property(index);
			auto userProp = widget->metaObject()->userProperty();
			prop.write(inst, userProp.read(widget));
		}
	}
}



#endif // DBWIDGETPAGE_H
