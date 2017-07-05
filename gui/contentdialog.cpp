#include "contentdialog.h"
#include <dialogmaster.h>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QMetaProperty>
#include <QSettings>

ContentDialog::ContentDialog(QWidget *parent) :
	QDialog(parent),
	tabWidget(new QTabWidget(this))
{
	DialogMaster::masterDialog(this, false, Qt::WindowContextHelpButtonHint);

	tabWidget->setTabBarAutoHide(true);

	auto layout = new QVBoxLayout(this);
	setLayout(layout);
	layout->addWidget(tabWidget);

	auto box = new QDialogButtonBox(this);
	layout->addWidget(box);
	box->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(box, &QDialogButtonBox::accepted,
			this, &ContentDialog::accept);
	connect(box, &QDialogButtonBox::rejected,
			this, &ContentDialog::reject);
}

QVariant ContentDialog::execute(QWidget *contentWidget, const QVariant &defaultValue, QWidget *parent)
{
	auto res = execute(contentWidget->windowTitle(), {contentWidget}, {defaultValue}, parent);
	if(res.isEmpty())
		return QVariant();
	else
		return res.first();
}

QVariantList ContentDialog::execute(const QString &windowTitle, const QList<QWidget *> &contentWidgets, const QList<QVariant> &defaultValues, QWidget *parent)
{
	ContentDialog dialog(parent);
	dialog.setWindowTitle(windowTitle);

	Q_ASSERT(contentWidgets.size() == defaultValues.size());
	Q_ASSERT(!contentWidgets.isEmpty());

	for(auto i = 0; i < contentWidgets.size(); i++) {
		auto contentWidget = contentWidgets[i];

		contentWidget->layout()->setContentsMargins(dialog.layout()->contentsMargins());
		dialog.tabWidget->addTab(contentWidget,
								 contentWidget->windowIcon(),
								 contentWidget->windowTitle());

		auto prop = contentWidget->metaObject()->userProperty();
		prop.write(contentWidget, defaultValues[i]);
	}

	QSettings settings;
	settings.beginGroup(QStringLiteral("gui/dialogs"));
	settings.beginGroup(contentWidgets.first()->objectName());
	if(settings.contains(QStringLiteral("geom")))
		dialog.restoreGeometry(settings.value(QStringLiteral("geom")).toByteArray());
	else
		dialog.adjustSize();

	QVariantList res;
	if(dialog.exec() == QDialog::Accepted) {
		foreach(auto contentWidget, contentWidgets) {
			auto prop = contentWidget->metaObject()->userProperty();
			res.append(prop.read(contentWidget));
		}
	}

	settings.setValue(QStringLiteral("geom"), dialog.saveGeometry());

	return res;
}
