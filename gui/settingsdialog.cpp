#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QCheckBox>
#include <QMetaProperty>
#include <QSpinBox>
#include <QDateTimeEdit>
#include <pluginloader.h>
#include <QKeySequenceEdit>
#include <QLineEdit>
#include <dialogmaster.h>
#include <QLabel>
#include <comboboxconfig.h>
#include <QComboBox>
#include <dbsettings.h>

SettingsDialog::SettingsDialog(QWidget *parent) :
	QDialog(parent),
	_ui(new Ui::SettingsDialog),
	_settingsWidgets()
{
	_ui->setupUi(this);
	DialogMaster::masterDialog(this);

	auto pal = _ui->generalScrollArea->palette();
	pal.setColor(QPalette::Window, _ui->tabWidget->palette().color(QPalette::Base));
	_ui->generalScrollArea->setPalette(pal);
	_ui->pluginScrollArea->setPalette(pal);

	QSettings localSettings;
	_ui->pluginComboBox->addItems(PluginLoader::availablePlugins());
	_ui->pluginComboBox->setCurrentText(localSettings.value(QStringLiteral("plugins/preferred"),
															PluginLoader::currentPlugin()).toString());

	auto libSettings = DbSettings::create(this);
	createWidgets(_ui->generalScrollAreaContents,
				  _ui->generalFormLayout,
				  libSettings,
				  {
					  {
						  tr("Use &GUI Installer"),
						  tr("If supported by the plugin, a GUI will be used instead of a console popup"),
						  QStringLiteral("lib/operations/usegui"),
						  QMetaType::Bool,
						  true
					  },
					  {
						  tr("Operation &Order"),
						  tr("Specify the order in which operations should be done"),
						  QStringLiteral("lib/operations/uninstall_first"),
						  qMetaTypeId<ComboboxConfig>(),
						  QVariant::fromValue<ComboboxConfig>({
							  {tr("Install then uninstall"), tr("Uninstall then install")},
							  {false, true},
							  false
						  })
					  },
					  {
						  tr("Default &Console"),
						  tr("Specify the console to use for operations. "
						  "If left empty, the TERM environment variable will be used"),
						  QStringLiteral("gui/operator/console"),
						  QMetaType::QString
					  }
				  });

	auto plugin = PluginLoader::plugin();
	auto settings = plugin->createSyncedSettings(this);
	createWidgets(_ui->pluginScrollAreaContents,
				  _ui->pluginFormLayout,
				  settings,
				  plugin->listSettings());

	restoreGeometry(localSettings.value(QStringLiteral("gui/settings/geom")).toByteArray());
}

SettingsDialog::~SettingsDialog()
{
	QSettings().setValue(QStringLiteral("gui/settings/geom"), saveGeometry());
	delete _ui;
}

void SettingsDialog::showSettings(QWidget *parent)
{
	SettingsDialog dialog(parent);
	dialog.exec();
}

void SettingsDialog::accept()
{
	QSet<QSettings*> syncable;

	QSettings localSettings;
	localSettings.setValue(QStringLiteral("plugins/preferred"), _ui->pluginComboBox->currentText());

	for(auto it = _settingsWidgets.constBegin(); it != _settingsWidgets.constEnd(); ++it) {
		auto cBox = qobject_cast<QComboBox*>(it->second);
		if(cBox) {
			auto data = cBox->currentData();
			if(!data.isValid())
				data = cBox->currentText();
			it->first->setValue(it.key(), data);
		} else {
			auto userProp = it->second->metaObject()->userProperty();
			it->first->setValue(it.key(), userProp.read(it->second));
			syncable.insert(it->first);
		}
	}

	foreach(auto settings, syncable)
		settings->sync();
	PluginLoader::plugin()->settingsChanged();
	QDialog::accept();
}

void SettingsDialog::createWidgets(QWidget *parent, QFormLayout *layout, QSettings *settings, const QList<PackageManagerPlugin::SettingsInfo> &infos)
{
	foreach (auto info, infos) {
		QWidget *widget = nullptr;

		if(!info.widgetClassName.isNull()) {
			auto metaObject = QMetaType::metaObjectForType(QMetaType::type(info.widgetClassName));
			if(metaObject) {
				auto object = metaObject->newInstance(Q_ARG(QWidget*, parent));
				widget = qobject_cast<QWidget*>(object);
				if(!widget && object)
					object->deleteLater();
			}
		}

		if(!widget) {
			if(info.type == qMetaTypeId<ComboboxConfig>()) {
				auto config = info.defaultValue.value<ComboboxConfig>();
				if(config.values.isEmpty())
					config.values = QVariant(config.displayNames).toList();
				auto cbox = new QComboBox(parent);
				for(auto i = 0; i < config.displayNames.size() && i < config.values.size(); i++)
					cbox->addItem(config.displayNames[i], config.values[i]);
				cbox->setCurrentIndex(qMax(0, config.values.indexOf(settings->value(info.settingsKeys, info.defaultValue))));
				cbox->setEditable(config.editable);
				widget = cbox;
			} else {
				switch (info.type) {
				case QMetaType::Bool:
					widget = new QCheckBox(parent);
					break;
				case QMetaType::Int:
					widget = new QSpinBox(parent);
					break;
				case QMetaType::Double:
					widget = new QDoubleSpinBox(parent);
					break;
				case QMetaType::QTime:
					widget = new QTimeEdit(parent);
					break;
				case QMetaType::QDate:
					widget = new QDateEdit(parent);
					break;
				case QMetaType::QDateTime:
					widget = new QDateTimeEdit(parent);
					break;
				case QMetaType::QKeySequence:
					widget = new QKeySequenceEdit(parent);
					break;
				case QMetaType::QString:
					widget = new QLineEdit(parent);
					break;
				default:
					widget = new QLineEdit(parent);
					break;
				}

				auto userProp = widget->metaObject()->userProperty();
				userProp.write(widget, settings->value(info.settingsKeys, info.defaultValue));
			}
		}

		if(widget->toolTip().isNull())
			widget->setToolTip(info.description);

		_settingsWidgets.insert(info.settingsKeys, {settings, widget});

		auto label = new QLabel(info.displayName + tr(":"), parent);
		label->setToolTip(info.description);
		label->setBuddy(widget);

		layout->addRow(label, widget);
	}
}
