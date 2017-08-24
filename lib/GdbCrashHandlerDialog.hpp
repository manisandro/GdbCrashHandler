/*
 * GdbCrashHandlerDialog.hpp
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

#ifndef GDBCRASHHANDLERDIALOG_HPP
#define GDBCRASHHANDLERDIALOG_HPP

#include "GdbCrashHandler.hpp"
#include "ui_GdbCrashHandlerDialog.h"
#include <QProcess>

class GdbCrashHandlerDialog : public QDialog {
	Q_OBJECT

public:
	explicit GdbCrashHandlerDialog(const GdbCrashHandler::Configuration& config, int pid, const QString& savefile, QWidget *parent = 0);

private:
	Ui::GdbCrashHandlerDialog ui;
	GdbCrashHandler::Configuration mConfig;
	int mPid;
	QProcess mGdbProcess;
	QPushButton* mSendButton = nullptr;

	void closeEvent(QCloseEvent *) override;

private slots:
	void appendGdbOutput();
	void handleGdbFinished(int exitCode, QProcess::ExitStatus exitStatus);
	void regenerateBacktrace();
	void sendReport();
};

#endif // GDBCRASHHANDLERDIALOG_HPP
