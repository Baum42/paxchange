#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <databasecontroller.h>
#include <qsingleinstance.h>
#include "traycontrol.h"
#include "pluginloader.h"
#include "databasewizard.h"
#include "consoleoperator.h"

static void setupParser(QCommandLineParser &parser);

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QSingleInstance instance;

	QCommandLineParser parser;
	setupParser(parser);
	parser.process(a);

	instance.setStartupFunction([&]() -> int {
		QApplication::setApplicationName(QStringLiteral(TARGET));
		QApplication::setApplicationVersion(QStringLiteral(VERSION));
		QApplication::setOrganizationName(QStringLiteral(COMPANY));
		QApplication::setOrganizationDomain(QStringLiteral(BUNDLE));
		QApplication::setApplicationDisplayName(QStringLiteral(DISPLAY_NAME));
		QApplication::setWindowIcon(QIcon(QStringLiteral(":/icons/main.svg")));
		QApplication::setQuitOnLastWindowClosed(false);

		try {
			PluginLoader::loadPlugin(parser.value(QStringLiteral("p")));
		} catch(PluginLoadException &e) {
			qCritical() << e.what();
			return EXIT_FAILURE;
		}

		auto tray = new TrayControl(&a);
		new ConsoleOperator(&a);

		if(!DatabaseController::instance()->isLoaded()) {
			if(!DatabaseWizard::run())
				return EXIT_SUCCESS;
		}

		tray->show();
		return 0;
	});

	QObject::connect(&instance, &QSingleInstance::instanceMessage, [&](QStringList args){
		if(!parser.parse(args) || !parser.isSet(QStringLiteral("f")))
			return;
		PluginLoader::plugin()->forwardedArguments(parser.positionalArguments());
	});

	return instance.singleExec();
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
	parser.addOption({
						 {QStringLiteral("f"), QStringLiteral("forward")},
						 QCoreApplication::translate("GLOBAL", "Forwards the arguments to the plugin")
					 });
}
