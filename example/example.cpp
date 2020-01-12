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
#include <QDir>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTemporaryFile>
#include <QTextStream>
#include <QThread>
#include <QVBoxLayout>

class CrashThread : public QThread {
private:
	void run() {
		QTextStream(stdout) << (*((int*)0)) << endl;
	}
};


CrashExample::CrashExample(const QString& fileName, QWidget *parent)
	: QDialog(parent)
{
	setWindowIcon(QIcon(":/icons/bug.png"));
	setWindowTitle("Crash Example");
	setLayout(new QVBoxLayout());
	mTextEdit = new QPlainTextEdit();
	QFile file;
	if(!fileName.isEmpty()) {
		file.setFileName(fileName);
		if(file.open(QIODevice::ReadOnly)) {
			mTextEdit->setPlainText(file.readAll());
		}
	}
	layout()->addWidget(mTextEdit);
	layout()->addWidget(new QLabel("Press \"Crash\" to make the application crash."));
	QDialogButtonBox* bbox = new QDialogButtonBox();
	QPushButton* crashButton = bbox->addButton("Crash", QDialogButtonBox::ActionRole);
	connect(crashButton, &QPushButton::clicked, this, &CrashExample::crash);
	QPushButton* crashThreadButton = bbox->addButton("Crash in thread", QDialogButtonBox::ActionRole);
	connect(crashThreadButton, &QPushButton::clicked, this, &CrashExample::crashInThread);
	layout()->addWidget(bbox);
}

void CrashExample::crash()
{
	QTextStream(stdout) << (*((int*)0)) << endl;
}

void CrashExample::crashInThread()
{
	CrashThread thread;
	thread.start();
	thread.wait();
}

QString CrashExample::crashSave() const
{
	QTemporaryFile file(QDir::home().absoluteFilePath("crashSave_XXXXXX.txt"));
	file.setAutoRemove(false);
	if(file.open()) {
		file.write(mTextEdit->toPlainText().toLocal8Bit());
		return file.fileName();
	}
	return QString();
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
	GdbCrashHandler::init(config);

	CrashExample example(argc > 1 ? argv[1] : "");
	GdbCrashHandler::setSaveCallback(std::bind(&CrashExample::crashSave, &example));
	example.show();
	return app.exec();
}
