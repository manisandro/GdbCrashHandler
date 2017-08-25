/*
 * GdbCrashHandler.cpp
 * Copyright (C) 2017 Sandro Mani <manisandro@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "GdbCrashHandler.hpp"
#include "GdbCrashHandlerDialog.hpp"

#include <csignal>
#include <cstring>
#include <iostream>
#include <QApplication>
#include <QCommandLineParser>
#ifdef Q_OS_LINUX
#include <sys/prctl.h>
#endif

namespace GdbCrashHandler {

static saveCallback_t gSaveCallback = nullptr;

static void signalHandler(int signal) {
	std::signal(signal, nullptr);

	QProcess process;
	QStringList args = QApplication::arguments();
	args.removeFirst(); // Remove program path
	if(gSaveCallback) {
		QString fileName = gSaveCallback();
		args << "--crashsavefile" << fileName;
	}
	args << "--crashhandle" << QString::number(QApplication::applicationPid());
	process.start(QApplication::applicationFilePath(), args);
#ifdef Q_OS_LINUX
	// Allow crash handler spawned debugger to attach to the crashed process
	prctl(PR_SET_PTRACER, process.pid(), 0, 0, 0);
#endif
	process.waitForFinished(-1);
	std::raise(signal);
}

#ifndef __ARMEL__
static void terminateHandler() {
	std::set_terminate(nullptr);
	std::exception_ptr exptr = std::current_exception();
	if (exptr != 0) {
		try {
			std::rethrow_exception(exptr);
		} catch (std::exception &ex) {
			std::cerr << "Terminated due to exception: " << ex.what() << std::endl;
		} catch (...) {
			std::cerr << "Terminated due to unknown exception" << std::endl;
		}
	} else {
		std::cerr << "Terminated due to unknown reason" << std::endl;
	}
	signalHandler(SIGABRT);
}
#endif

void init(const Configuration& config, saveCallback_t saveCallback)
{
	QCommandLineParser parser;
	QCommandLineOption crashHandleOption("crashhandle");
	crashHandleOption.setValueName("pid");
	parser.addOption(crashHandleOption);
	QCommandLineOption saveFileOption("crashsavefile");
	saveFileOption.setValueName("path");
	parser.addOption(saveFileOption);
	parser.process(*qApp);

	if(parser.isSet(crashHandleOption)) {
		int pid = parser.value(crashHandleOption).toInt();
		QString saveFile;
		if(parser.isSet(saveFileOption)) {
			saveFile = parser.value(saveFileOption);
		}
		GdbCrashHandlerDialog dialog(config, pid, saveFile);
		dialog.show();
		int status = qApp->exec();
		if(dialog.restartApplication()) {
			QStringList arguments = qApp->arguments();
			arguments.removeFirst(); // Remove program path
			int pos = arguments.indexOf("--crashhandle");
			// Remove both key and value
			arguments.removeAt(pos);
			arguments.removeAt(pos);
			pos = arguments.indexOf("--crashsavefile");
			if(pos != 1) {
				arguments.removeAt(pos);
				arguments.removeAt(pos);
			}
			QProcess::startDetached(QApplication::applicationFilePath(), arguments);
		}
		std::exit(status);
	} else {
		gSaveCallback = saveCallback;
		std::signal(SIGSEGV, signalHandler);
		std::signal(SIGABRT, signalHandler);
	#ifndef __ARMEL__
		std::set_terminate(terminateHandler);
	#endif
	}
}

void setSaveCallback(saveCallback_t saveCallback)
{
	gSaveCallback = saveCallback;
}

}
