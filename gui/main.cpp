#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QTemporaryFile>
#include <databasecontroller.h>
#include <qsingleinstance.h>
#include <QFileInfo>
#include <QFile>
#include <QTranslator>
#include <QLibraryInfo>
#include "traycontrol.h"
#include "pluginloader.h"
#include "wizard/databasewizard.h"
#include "consoleoperator.h"

static void setupParser(QCommandLineParser &parser);
static void cacheForwardedPluginArgs(QStringList args);
static void readCachedForwardedPluginArgs();

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

	//load translations
	auto translator = new QTranslator(qApp);
	if(translator->load(QLocale(),
						QStringLiteral("pacsync_gui"),
						QStringLiteral("_"),
						QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
		qApp->installTranslator(translator);
	else {
		qWarning() << "Failed to load translations";
		delete translator;
	}

	QSingleInstance instance;

	QCommandLineParser parser;
	setupParser(parser);
	parser.process(a);

	instance.setStartupFunction([&]() {
		if(parser.isSet(QStringLiteral("f"))){
			cacheForwardedPluginArgs(parser.positionalArguments());

			QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection); //DEBUG
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
				QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection); //DEBUG
				return EXIT_SUCCESS;
			}
		}

		readCachedForwardedPluginArgs();

		tray->show();
		return EXIT_SUCCESS;
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

static void cacheForwardedPluginArgs(QStringList args)
{
	auto cacheDir = QDir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
	cacheDir.mkpath(QStringLiteral("hooks"));
	cacheDir.cd(QStringLiteral("hooks"));

	QTemporaryFile file(cacheDir.absoluteFilePath(QStringLiteral("XXXXXX.cmd")));
	file.setAutoRemove(false);
	file.open();
	QDataStream stream(&file);
	stream << args;
	file.close();
}

static void readCachedForwardedPluginArgs()
{
	auto cacheDir = QDir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
	if(cacheDir.cd(QStringLiteral("hooks"))){
		foreach (auto fileInfo, cacheDir.entryInfoList(QDir::Files)) {
			QFile file(fileInfo.absoluteFilePath());
			file.open(QIODevice::ReadOnly);
			QDataStream stream(&file);
			QStringList args;
			stream >> args;
			QMetaObject::invokeMethod(PluginLoader::plugin(), "forwardedArguments",
									   Qt::QueuedConnection,
									   Q_ARG(QStringList, args));
			file.close();
			file.remove();
		}
	}
}
