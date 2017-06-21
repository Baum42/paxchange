#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <databasecontroller.h>
#include "traycontrol.h"
#include "pluginloader.h"
#include "databasewizard.h"
#include "consoleoperator.h"

static void setupParser(QCommandLineParser &parser);

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QApplication::setApplicationName(QStringLiteral(TARGET));
	QApplication::setApplicationVersion(QStringLiteral(VERSION));
	QApplication::setOrganizationName(QStringLiteral(COMPANY));
	QApplication::setOrganizationDomain(QStringLiteral(BUNDLE));
	QApplication::setApplicationDisplayName(QStringLiteral(DISPLAY_NAME));
	QApplication::setWindowIcon(QIcon(QStringLiteral(":/icons/main.svg")));
	QApplication::setQuitOnLastWindowClosed(false);

	QCommandLineParser parser;
	setupParser(parser);
	parser.process(a);

	try {
		PluginLoader::loadPlugin(parser.value(QStringLiteral("p")));
	} catch(PluginLoadException &e) {
		qCritical() << e.what();
		return EXIT_FAILURE;
	}

	if(!DatabaseController::instance()->isLoaded()) {
		if(!DatabaseWizard::run())
			return EXIT_SUCCESS;
	}

	TrayControl tray;
	ConsoleOperator co;
	return a.exec();
}

static void setupParser(QCommandLineParser &parser)
{
	parser.addVersionOption();
	parser.addHelpOption();

	parser.addOption({
						 {QStringLiteral("p"), QStringLiteral("pacin")},
						 QCoreApplication::translate("GLOBAL", "Explicitly select the plugin to be loaded"),
						 QStringLiteral("plugin")
					 });
}
