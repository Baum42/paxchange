#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QCheckBox>
#include <QMetaProperty>
#include <QSpinBox>
#include <QDateTimeEdit>
#include <pluginloader.h>
#include <QKeySequenceEdit>
#include <QLineEdit>

SettingsDialog::SettingsDialog(QWidget *parent) :
	QDialog(parent),
	_ui(new Ui::SettingsDialog),
	_settingsWidgets()
{
	_ui->setupUi(this);
	PackageManagerPlugin* plugin = PluginLoader::plugin();
	auto settings = plugin->createPluginSettings(this);

	foreach (auto info, plugin->listSettings()) {
		QWidget *widget = nullptr;

		if(!info.widgetClassName.isNull()) {
			auto metaObject = QMetaType::metaObjectForType(QMetaType::type(info.widgetClassName));
			if(metaObject) {
				auto object = metaObject->newInstance(Q_ARG(QWidget*, _ui->pluginTab));
				widget = qobject_cast<QWidget*>(object);
				if(!widget && object)
					object->deleteLater();
			}
		}

		if(!widget) {
			switch (info.type) {
			case QMetaType::Bool:
				widget = new QCheckBox(_ui->pluginTab);
				break;
			case QMetaType::Int:
				widget = new QSpinBox(_ui->pluginTab);
				break;
			case QMetaType::Double:
				widget = new QDoubleSpinBox(_ui->pluginTab);
				break;
			case QMetaType::QTime:
				widget = new QTimeEdit(_ui->pluginTab);
				break;
			case QMetaType::QDate:
				widget = new QDateEdit(_ui->pluginTab);
				break;
			case QMetaType::QDateTime:
				widget = new QDateTimeEdit(_ui->pluginTab);
				break;
			case QMetaType::QKeySequence:
				widget = new QKeySequenceEdit(_ui->pluginTab);
				break;
			case QMetaType::QString:
				widget = new QLineEdit(_ui->pluginTab);
				break;
			default:
				widget = new QLineEdit(_ui->pluginTab);
				break;
			}
		}

		if(widget->toolTip().isNull())
			widget->setToolTip(info.description);
		_settingsWidgets.insert(info.settingsKeys, widget);
		_ui->pluginFormLayout->addRow(info.displayName + tr(":"), widget);
	}

	settings->deleteLater();
}

SettingsDialog::~SettingsDialog()
{
	delete _ui;
}

void SettingsDialog::showSettings(QWidget *parent)
{
	SettingsDialog dialog(parent);
	dialog.exec();
}

void SettingsDialog::accept()
{
	auto plugin = PluginLoader::plugin();
	auto settings = plugin->createPluginSettings(this);

	for(auto it = _settingsWidgets.constBegin(); it != _settingsWidgets.constEnd(); ++it) {
		auto userProp = it.value()->metaObject()->userProperty();
		settings->setValue(it.key(), userProp.read(it.value()));
	}

	settings->sync();
	plugin->settingsChanged();
	QDialog::accept();
	settings->deleteLater();
}
