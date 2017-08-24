/*
 * example.cpp
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

#include "example.hpp"

#include <GdbCrashHandler.hpp>
#include <QApplication>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QTextStream>
#include <QVBoxLayout>


CrashExample::CrashExample(QWidget *parent)
	: QDialog(parent)
{
	setWindowIcon(QIcon(":/icons/bug.png"));
	setWindowTitle("Crash Example");
	setLayout(new QVBoxLayout());
	layout()->addWidget(new QLabel("Press \"Crash\" to make the application crash."));
	QDialogButtonBox* bbox = new QDialogButtonBox();
	QPushButton* crashButton = bbox->addButton("Crash", QDialogButtonBox::ActionRole);
	connect(crashButton, &QPushButton::clicked, this, &CrashExample::crash);
	layout()->addWidget(bbox);
}

void CrashExample::crash()
{
	QTextStream(stdout) << (*((int*)0)) << endl;
}


int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	GdbCrashHandler::Configuration config;
	config.applicationName = "CrashExample";
	config.applicationVersion = "1.0";
	config.applicationIcon = ":/icons/bug.png";
	config.submitMethod = GdbCrashHandler::Configuration::SubmitService;
	config.submitAddress = "http://127.0.0.1/report.php";
	GdbCrashHandler::init(argc, argv, config);

	CrashExample example;
	example.show();
	return app.exec();
}
