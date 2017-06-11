#include "contentdialog.h"
#include <dialogmaster.h>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QMetaProperty>
#include <QSettings>

ContentDialog::ContentDialog(QWidget *parent) :
	QDialog(parent)
{
	DialogMaster::masterDialog(this);

	auto layout = new QVBoxLayout(this);
	setLayout(layout);

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
	ContentDialog dialog(parent);
	dialog.setWindowTitle(contentWidget->windowTitle());
	dialog.setWindowIcon(contentWidget->windowIcon());

	contentWidget->setParent(&dialog);
	static_cast<QVBoxLayout*>(dialog.layout())->insertWidget(0, contentWidget);

	auto prop = contentWidget->metaObject()->userProperty();
	prop.write(contentWidget, defaultValue);

	QSettings settings;
	settings.beginGroup(QStringLiteral("contentDialog"));
	if(settings.contains(contentWidget->objectName()))
		dialog.restoreGeometry(settings.value(contentWidget->objectName()).toByteArray());
	else
		dialog.adjustSize();

	QVariant res;
	if(dialog.exec() == QDialog::Accepted)
		res = prop.read(contentWidget);

	settings.setValue(contentWidget->objectName(), dialog.saveGeometry());

	return res;
}
