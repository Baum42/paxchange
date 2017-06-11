#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include "pluginloader.h"
#include "editpackagesdialog.h"

static void setupParser(QCommandLineParser &parser);

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QApplication::setQuitOnLastWindowClosed(false);

	QCommandLineParser parser;
	setupParser(parser);
	parser.process(a);

	try {
		PluginLoader::loadPlugin(parser.value("p"));
	} catch(PluginLoadException &e) {
		qCritical() << e.what();
		return EXIT_FAILURE;
	}

	qDebug() << EditPackagesDialog::editPackages({"nano"});
	//TODO return a.exec();
	return 0;
}

static void setupParser(QCommandLineParser &parser)
{
	parser.addVersionOption();
	parser.addHelpOption();

	parser.addOption({
						 {"p", "pacin"},
						 "Explicitly select the plugin to be loaded",
						 "plugin"
					 });
}
