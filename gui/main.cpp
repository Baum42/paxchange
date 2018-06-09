#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <databasecontroller.h>
#include <qsingleinstance.h>
#include <QFileInfo>
#include <QFile>
#include "traycontrol.h"
#include "pluginloader.h"
#include "wizard/databasewizard.h"
#include "consoleoperator.h"

static void setupParser(QCommandLineParser &parser);

int main(int argc, char *argv[])
{
	// ugly fix to enable stdin input
	QByteArrayList xArgs;
	int stdIdx = -1;
	for(auto i = 0; i < argc; i++) {
		if(qstrcmp(argv[i], "--stdin") == 0) {
			stdIdx = i;
			QFile in;
			in.open(stdin, QIODevice::ReadOnly);
			xArgs = in.readAll().simplified().split(' ');
			break;
		}
	}

	QVector<char*> nArgs{argc + xArgs.size()};
	int nArgc = 0;
	char **nArgv = nullptr;
	if(xArgs.isEmpty()) {
		nArgs.clear();
		nArgc = argc;
		nArgv = argv;
	} else {
		int idx;
		for(idx = 0; idx < argc; idx++)
			nArgs[idx] = argv[idx];
		for(; (idx - argc) < xArgs.size(); idx++)
			nArgs[idx] = xArgs[idx - argc].data();
		nArgs.removeAt(stdIdx);

		nArgc = nArgs.size();
		nArgv = nArgs.data();
	}

	// begin actual main
	QApplication a(nArgc, nArgv);
	QApplication::setApplicationName(QStringLiteral(TARGET));
	QApplication::setApplicationVersion(QStringLiteral(VERSION));
	QApplication::setOrganizationName(QStringLiteral(COMPANY));
	QApplication::setOrganizationDomain(QStringLiteral(BUNDLE));
	QApplication::setApplicationDisplayName(QStringLiteral(DISPLAY_NAME));
	QApplication::setWindowIcon(QIcon(QStringLiteral(":/icons/main.svg")));
	QApplication::setQuitOnLastWindowClosed(false);

	//load translations
	DatabaseController::loadTranslation(QStringLiteral("paxchange_gui"));
	QCommandLineParser parser;
	setupParser(parser);
	parser.process(a);

	QSingleInstance instance;
	instance.setGlobal(true);
	instance.setStartupFunction([&]() {
		if(parser.isSet(QStringLiteral("f"))){
			if(PluginLoader::cacheForwardedPluginArgs(parser.positionalArguments()))
				QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
			else
				return EXIT_FAILURE;
			return EXIT_SUCCESS;
		}

		try {
			PluginLoader::loadPlugin(parser.value(QStringLiteral("p")));
		} catch(PluginLoadException &e) {
			qCritical() << e.what();
			return EXIT_FAILURE;
		}

		auto tray = new TrayControl(&a);
		new ConsoleOperator(&a);

		if(!DatabaseController::instance()->isLoaded()) {
			if(!DatabaseWizard::run()) {
				QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
				return EXIT_SUCCESS;
			}
		} else
			PluginLoader::readCachedForwardedPluginArgs();

		tray->show();
		return EXIT_SUCCESS;
	});

	QObject::connect(&instance, &QSingleInstance::instanceMessage, [&](const QStringList &args){
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
	parser.addOption({
						 {QStringLiteral("stdin")},
						 QCoreApplication::translate("GLOBAL", "Arguments to be forwarded to the plugin should be read from stdin "
						 "and appended to the commandline")
					 });
}
