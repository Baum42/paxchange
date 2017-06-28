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
#include <databasecontroller.h>

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

	createWidgets(_ui->generalScrollAreaContents,
				  _ui->generalFormLayout,
				  false,
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
	createWidgets(_ui->pluginScrollAreaContents,
				  _ui->pluginFormLayout,
				  true,
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
	QSettings localSettings;
	auto nValue = _ui->pluginComboBox->currentText();
	if(nValue != localSettings.value((QStringLiteral("plugins/preferred")))) {
		localSettings.setValue(QStringLiteral("plugins/preferred"), _ui->pluginComboBox->currentText());
		DialogMaster::information(this,
								  tr("Please restart the application to apply the changed default plugin!"),
								  tr("Plugin changed"));
	}

	QVariantHash changes;
	for(auto it = _settingsWidgets.constBegin(); it != _settingsWidgets.constEnd(); ++it) {
		auto cBox = qobject_cast<QComboBox*>(it.value());
		if(cBox) {
			auto data = cBox->currentData();
			if(!data.isValid())
				data = cBox->currentText();
			changes.insert(it.key(), data);
		} else {
			auto userProp = it.value()->metaObject()->userProperty();
			changes.insert(it.key(), userProp.read(it.value()));
		}
	}

	DatabaseController::instance()->writeSettings(changes);
	PluginLoader::plugin()->settingsChanged();
	QDialog::accept();
}

void SettingsDialog::createWidgets(QWidget *parent, QFormLayout *layout, bool asPlugin, const QList<PackageManagerPlugin::SettingsInfo> &infos)
{
	auto ctr = DatabaseController::instance();

	foreach (auto info, infos) {
		auto key = asPlugin ?
					   SyncedSettings::pluginKey(info.settingsKeys) :
					   info.settingsKeys;
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
				cbox->setCurrentIndex(qMax(0, config.values.indexOf(ctr->readSettings(key, info.defaultValue))));
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
				userProp.write(widget, ctr->readSettings(key, info.defaultValue));
			}
		}

		if(widget->toolTip().isNull())
			widget->setToolTip(info.description);

		_settingsWidgets.insert(key, widget);

		auto label = new QLabel(info.displayName + tr(":"), parent);
		label->setToolTip(info.description);
		label->setBuddy(widget);

		layout->addRow(label, widget);
	}
}
