/*
 * GdbGdbCrashHandlerDialog.cpp
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

#include "GdbCrashHandlerDialog.hpp"
#include <QBuffer>
#include <QCryptographicHash>
#include <QDesktopServices>
#include <QHttpMultiPart>
#include <QMessageBox>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QPushButton>
#include <QTextStream>
#include <QTimer>
#include <QUrlQuery>
#include <QUuid>
#include <quazip/quazipfile.h>

GdbCrashHandlerDialog::GdbCrashHandlerDialog(const GdbCrashHandler::Configuration& config, int pid, const QString& savefile, QWidget *parent)
	: QDialog(parent)
	, mConfig(config)
	, mPid(pid)
{
	ui.setupUi(this);
	setWindowIcon(QIcon(config.applicationIcon));

	if(!savefile.isEmpty()) {
		ui.labelAutosave->setText(tr("<b>Your work was saved to <i>%1</i>.</b>").arg(savefile));
	}

	ui.labelIcon->setPixmap(QPixmap(config.applicationIcon));
	ui.labelIcon->setScaledContents(true);
	ui.labelTitle->setText(ui.labelTitle->text().arg(config.applicationName));

	ui.frameDetails->setVisible(false);
	ui.frameInfo->setVisible(false);

	connect(ui.checkBoxDetails, &QCheckBox::toggled, ui.frameDetails, &QFrame::setVisible);
	connect(ui.checkBoxInfo, &QCheckBox::toggled, ui.frameInfo, &QFrame::setVisible);
	connect(ui.buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(ui.buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
	connect(ui.buttonBox, &QDialogButtonBox::rejected, this, &GdbCrashHandlerDialog::killGdb);

	mSendButton = ui.buttonBox->addButton(tr("Send report"), QDialogButtonBox::ActionRole);
	connect(mSendButton, &QPushButton::clicked, this, &GdbCrashHandlerDialog::sendReport);
	ui.buttonBox->addButton(tr("Don't send report"), QDialogButtonBox::RejectRole);

	connect(ui.pushButtonRegenerate, &QPushButton::clicked, this, &GdbCrashHandlerDialog::regenerateBacktrace);

	mGdbProcess.setProcessChannelMode(QProcess::SeparateChannels);
	connect(&mGdbProcess, &QProcess::readyReadStandardOutput, this, &GdbCrashHandlerDialog::appendGdbOutput);
	connect(&mGdbProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(handleGdbFinished(int,QProcess::ExitStatus)));

	regenerateBacktrace();
	mSendButton->setDefault(true);

	resize(QSize(width(), sizeHint().height()));
}

void GdbCrashHandlerDialog::closeEvent(QCloseEvent *) {
	mGdbProcess.kill();
}

void GdbCrashHandlerDialog::appendGdbOutput() {
	ui.plainTextEditBacktrace->appendPlainText(mGdbProcess.readAllStandardOutput());
}

void GdbCrashHandlerDialog::handleGdbFinished(int exitCode, QProcess::ExitStatus exitStatus) {
	ui.widgetBusy->setVisible(false);
	ui.labelBusyText->setText("");
	ui.progressBarBacktrace->setVisible(false);
	ui.pushButtonRegenerate->setVisible(true);
	if(exitCode != 0 || exitStatus != QProcess::NormalExit) {
		ui.plainTextEditBacktrace->appendPlainText(tr("Failed to obtain backtrace. Is gdb installed?"));
	} else {
		QStringList lines = ui.plainTextEditBacktrace->toPlainText().split("\n", QString::SkipEmptyParts);
		ui.plainTextEditBacktrace->setPlainText(lines[0]);
		ui.plainTextEditBacktrace->appendPlainText("\n");
		for(int i = 1, n = lines.length(); i < n; ++i) {
			if(lines[i].startsWith("Thread")) {
				ui.plainTextEditBacktrace->appendPlainText("\n");
				ui.plainTextEditBacktrace->appendPlainText(lines[i]);
			} else if(lines[i].startsWith('#')) {
				ui.plainTextEditBacktrace->appendPlainText(lines[i]);
			}
		}
	}
	QTextCursor c = ui.plainTextEditBacktrace->textCursor();
	c.movePosition(QTextCursor::Start);
	ui.plainTextEditBacktrace->setTextCursor(c);
	ui.plainTextEditBacktrace->ensureCursorVisible();
}

void GdbCrashHandlerDialog::regenerateBacktrace() {
	ui.pushButtonRegenerate->setVisible(false);
	ui.progressBarBacktrace->setVisible(true);
	ui.labelBusyText->setText(tr("Gathering information..."));
	ui.widgetBusy->setVisible(true);
	ui.plainTextEditBacktrace->clear();

	mGdbProcess.start("gdb", QStringList() << "-p" << QString::number(mPid));
	mGdbProcess.write("set pagination off\n");
	mGdbProcess.write(mConfig.gdbBacktraceCommand.isEmpty() ? "thread apply all bt\n" : mConfig.gdbBacktraceCommand.toLocal8Bit());
	mGdbProcess.write("quit\n");
}

static void addDataPart(QHttpMultiPart* multiPart, const QByteArray& name, const QByteArray& data)
{
	QHttpPart dataPart;
	dataPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"" + name + "\""));
	dataPart.setBody(data);
	multiPart->append(dataPart);
}

void GdbCrashHandlerDialog::sendReport()
{
	setEnabled(false);
	setCursor(Qt::WaitCursor);
	if(mGdbProcess.state() != QProcess::NotRunning) {
		// wait for gdb to finish
		QEventLoop evLoop;
		connect(&mGdbProcess, SIGNAL(finished(int)), &evLoop, SLOT(quit()));
		evLoop.exec(QEventLoop::ExcludeUserInputEvents);
	}
	ui.labelBusyText->setText(tr("Sending report..."));
	ui.widgetBusy->setVisible(true);
	if(mConfig.submitMethod == GdbCrashHandler::Configuration::SubmitService && !mConfig.submitAddress.isEmpty()) {
		QByteArray report;
		QBuffer buf(&report);
		QuaZip quazip(&buf);
		quazip.open(QuaZip::mdCreate);
		QuaZipFile quazipfile(&quazip);
		QuaZipNewInfo info("stacktrace.txt");
		info.setPermissions( QFile::ReadOwner | QFile::ReadUser | QFile::ReadGroup | QFile::ReadOther );
		quazipfile.open(QIODevice::WriteOnly, info);
		quazipfile.write(ui.plainTextEditBacktrace->toPlainText().toLocal8Bit());
		quazipfile.close();
		quazip.close();

		QCryptographicHash crypt(QCryptographicHash::Md5);
		crypt.addData(report);

		QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

		QHttpPart filePart;
		filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"crashrpt\"; filename=\"stacktrace.zip\""));
		filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
		filePart.setBody(report);
		multiPart->append(filePart);

		addDataPart(multiPart, "appname", mConfig.applicationName.toLocal8Bit());
		addDataPart(multiPart, "appversion", QString("%1").arg(mConfig.applicationVersion).toLocal8Bit());
		addDataPart(multiPart, "emailfrom", ui.lineEditContact->text().toLocal8Bit());
		addDataPart(multiPart, "emailsubject", QString("%1 %2 Error Report").arg(mConfig.applicationName).arg(mConfig.applicationVersion).toLocal8Bit());
		addDataPart(multiPart, "crashguid", QUuid::createUuid().toString().mid(1, 36).toLocal8Bit());
		addDataPart(multiPart, "description", ui.plainTextEditErrorDescr->toPlainText().toLocal8Bit());
		addDataPart(multiPart, "crashrpt", report);
		addDataPart(multiPart, "md5", crypt.result().toHex());

		QNetworkAccessManager nam;
		QEventLoop evLoop;
		QTimer timer;
		timer.setSingleShot(true);

		QNetworkRequest request(mConfig.submitAddress);
		QNetworkReply* reply = nam.post(request, multiPart);
		timer.start(10000);
		connect(reply, &QNetworkReply::finished, &evLoop, &QEventLoop::quit);
		connect(&timer, &QTimer::timeout, reply, &QNetworkReply::abort);
		evLoop.exec();

		if(!timer.isActive()) {
			QMessageBox::critical(this, tr("Submit failed"), tr("The attempt to submit the report timed out."));
		} else if(reply->error() != QNetworkReply::NoError) {
			QMessageBox::critical(this, tr("Submit failed"), tr("The attempt to submit the report failed: %1").arg(reply->errorString()));
		} else {
			// Success
			accept();
		}
		reply->deleteLater();
	} else {
		QMessageBox::critical(this, tr("Submit failed"), tr("No submit method was configured by the application."));
	}
	setEnabled(true);
	unsetCursor();
	ui.widgetBusy->setVisible(false);
	ui.labelBusyText->setText("");
}

void GdbCrashHandlerDialog::killGdb()
{
	mGdbProcess.kill();
}
