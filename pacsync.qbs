import qbs

Project {
	minimumQbsVersion: "1.8.0"

	DynamicLibrary {
		id: pslib
		name: "pacsync"

		Depends { name: "cpp" }
		Depends { name: "Qt"; submodules: ["core", "jsonserializer"] }

		files: [
			 "lib/packageplugin.cpp",
			 "lib/packageplugin.h",
		 ]

		Group {     // Properties for the produced executable
			fileTagsFilter: product.type
			qbs.install: true
			qbs.installDir: "lib"
		}
	}

	CppApplication {
		id: psgui
		name: "pacsync-gui"
		files:[
			"gui/main.cpp",
			"gui/mainwindow.cpp",
			"gui/mainwindow.h",
			"gui/mainwindow.ui"
		]

		Depends { name: "pacsync" }
		Depends { name: "Qt"; submodules: ["core", "gui", "widgets"] }

		cpp.includePaths: ["../lib"]
		cpp.libraryPaths: [pslib.buildDirectory]
		cpp.dynamicLibraries: "pacsync"

		Group {     // Properties for the produced executable
			fileTagsFilter: product.type
			qbs.install: true
		}
	}
}
