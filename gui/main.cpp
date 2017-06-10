#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include "pluginloader.h"

static void setupParser(QCommandLineParser &parser);

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QCommandLineParser parser;
	setupParser(parser);
	parser.process(a);

	PluginLoader loader;
	PackageManagerPlugin *plugin = nullptr;
	try {
		auto key = parser.value("p");
		if(key.isEmpty())
			key = loader.defaultPlugin();
		plugin = loader.loadPlugin(key);
	} catch(PluginLoadException &e) {
		qDebug() << e.what();
		return EXIT_FAILURE;
	}

	MainWindow w(plugin);
	w.show();

	return a.exec();
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
