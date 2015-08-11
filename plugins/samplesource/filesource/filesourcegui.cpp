///////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2015 Edouard Griffiths, F4EXB                                   //
//                                                                               //
// This program is free software; you can redistribute it and/or modify          //
// it under the terms of the GNU General Public License as published by          //
// the Free Software Foundation as version 3 of the License, or                  //
//                                                                               //
// This program is distributed in the hope that it will be useful,               //
// but WITHOUT ANY WARRANTY; without even the implied warranty of                //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                  //
// GNU General Public License V3 for more details.                               //
//                                                                               //
// You should have received a copy of the GNU General Public License             //
// along with this program. If not, see <http://www.gnu.org/licenses/>.          //
///////////////////////////////////////////////////////////////////////////////////

#include <QDebug>

#include <QTime>
#include <QDateTime>
#include <QString>
#include <QFileDialog>
#include "ui_filesourcegui.h"
#include "plugin/pluginapi.h"
#include "gui/colormapper.h"
#include "mainwindow.h"

#include "filesourcegui.h"

FileSourceGui::FileSourceGui(PluginAPI* pluginAPI, QWidget* parent) :
	QWidget(parent),
	ui(new Ui::FileSourceGui),
	m_pluginAPI(pluginAPI),
	m_settings(),
	m_sampleSource(NULL),
	m_acquisition(false),
	m_fileName("..."),
	m_sampleRate(0),
	m_centerFrequency(0),
	m_startingTimeStamp(0),
	m_samplesCount(0),
	m_tickCount(0)
{
	ui->setupUi(this);
	ui->centerFrequency->setColorMapper(ColorMapper(ColorMapper::ReverseGold));
	ui->centerFrequency->setValueRange(7, 0, pow(10,7));
	ui->fileNameText->setText(m_fileName);
	connect(&m_updateTimer, SIGNAL(timeout()), this, SLOT(updateHardware()));
	connect(&(m_pluginAPI->getMainWindow()->getMasterTimer()), SIGNAL(timeout()), this, SLOT(tick()));
	displaySettings();

	m_sampleSource = new FileSourceInput(m_pluginAPI->getMainWindowMessageQueue(), m_pluginAPI->getMainWindow()->getMasterTimer());
	m_pluginAPI->setSampleSource(m_sampleSource);
}

FileSourceGui::~FileSourceGui()
{
	delete ui;
}

void FileSourceGui::destroy()
{
	delete this;
}

void FileSourceGui::setName(const QString& name)
{
	setObjectName(name);
}

QString FileSourceGui::getName() const
{
	return objectName();
}

void FileSourceGui::resetToDefaults()
{
	m_generalSettings.resetToDefaults();
	m_settings.resetToDefaults();
	displaySettings();
	sendSettings();
}

QByteArray FileSourceGui::serializeGeneral() const
{
	return m_generalSettings.serialize();
}

bool FileSourceGui::deserializeGeneral(const QByteArray&data)
{
	if(m_generalSettings.deserialize(data)) {
		displaySettings();
		sendSettings();
		return true;
	} else {
		resetToDefaults();
		return false;
	}
}

qint64 FileSourceGui::getCenterFrequency() const
{
	return m_generalSettings.m_centerFrequency;
}

QByteArray FileSourceGui::serialize() const
{
	return m_settings.serialize();
}

bool FileSourceGui::deserialize(const QByteArray& data)
{
	if(m_settings.deserialize(data)) {
		displaySettings();
		sendSettings();
		return true;
	} else {
		resetToDefaults();
		return false;
	}
}

bool FileSourceGui::handleMessage(Message* message)
{
	if(FileSourceInput::MsgReportFileSourceAcquisition::match(message))
	{
		m_acquisition = ((FileSourceInput::MsgReportFileSourceAcquisition*)message)->getAcquisition();
		updateWithAcquisition();
		message->completed();
		return true;
	}
	else if(FileSourceInput::MsgReportFileSourceStreamData::match(message))
	{
		m_sampleRate = ((FileSourceInput::MsgReportFileSourceStreamData*)message)->getSampleRate();
		m_centerFrequency = ((FileSourceInput::MsgReportFileSourceStreamData*)message)->getCenterFrequency();
		m_startingTimeStamp = ((FileSourceInput::MsgReportFileSourceStreamData*)message)->getStartingTimeStamp();
		message->completed();
		updateWithStreamData();
		return true;
	}
	else if(FileSourceInput::MsgReportFileSourceStreamTiming::match(message))
	{
		m_samplesCount = ((FileSourceInput::MsgReportFileSourceStreamTiming*)message)->getSamplesCount();
		message->completed();
		updateWithStreamTime();
		return true;
	}
	else
	{
		return false;
	}
}

void FileSourceGui::displaySettings()
{
}

void FileSourceGui::sendSettings()
{
	/*
	if(!m_updateTimer.isActive())
		m_updateTimer.start(100);
		*/
}

void FileSourceGui::updateHardware()
{
	/*
	FileSourceInput::MsgConfigureFileSource* message = FileSourceInput::MsgConfigureFileSource::create(m_generalSettings, m_settings);
	message->submit(m_pluginAPI->getDSPEngineMessageQueue());
	m_updateTimer.stop();*/
}

void FileSourceGui::on_play_toggled(bool checked)
{
	FileSourceInput::MsgConfigureFileSourceWork* message = FileSourceInput::MsgConfigureFileSourceWork::create(checked);
	message->submit(m_pluginAPI->getDSPEngineMessageQueue());
}

void FileSourceGui::on_showFileDialog_clicked(bool checked)
{
	QString fileName = QFileDialog::getOpenFileName(this,
	    tr("Open I/Q record file"), ".", tr("SDR I/Q Files (*.sdriq)"));
	if (fileName != "") {
		m_fileName = fileName;
		ui->fileNameText->setText(m_fileName);
		configureFileName();
	}
}

void FileSourceGui::configureFileName()
{
	qDebug() << "FileSourceGui::configureFileName: " << m_fileName.toStdString().c_str();
	FileSourceInput::MsgConfigureFileSourceName* message = FileSourceInput::MsgConfigureFileSourceName::create(m_fileName);
	message->submit(m_pluginAPI->getDSPEngineMessageQueue());
}

void FileSourceGui::updateWithAcquisition()
{
	ui->play->setEnabled(m_acquisition);
	ui->play->setChecked(m_acquisition);
	ui->showFileDialog->setEnabled(!m_acquisition);
}

void FileSourceGui::updateWithStreamData()
{
	ui->centerFrequency->setValue(m_centerFrequency/1000);
	QString s = QString::number(m_sampleRate/1000.0, 'f', 0);
	ui->sampleRateText->setText(tr("%1k").arg(s));
	ui->play->setEnabled(m_acquisition);
	updateWithStreamTime(); // TODO: remove when time data is implemented
}

void FileSourceGui::updateWithStreamTime()
{
	int t_sec = 0;
	int t_msec = 0;

	if (m_sampleRate > 0){
		t_msec = ((m_samplesCount * 1000) / m_sampleRate) % 1000;
		t_sec = m_samplesCount / m_sampleRate;
	}

	QTime t(0, 0, 0, 0);
	t = t.addSecs(t_sec);
	t = t.addMSecs(t_msec);
	QString s_time = t.toString("hh:mm:ss.zzz");
	ui->relTimeText->setText(s_time);

	quint64 startingTimeStampMsec = m_startingTimeStamp * 1000;
	QDateTime dt = QDateTime::fromMSecsSinceEpoch(startingTimeStampMsec);
	dt = dt.addSecs(t_sec);
	dt = dt.addMSecs(t_msec);
	QString s_date = dt.toString("yyyyMMdd hh.mm.ss.zzz");
	ui->absTimeText->setText(s_date);
}

void FileSourceGui::tick()
{
	if ((++m_tickCount & 0xf) == 0) {
		FileSourceInput::MsgConfigureFileSourceStreamTiming* message = FileSourceInput::MsgConfigureFileSourceStreamTiming::create();
		message->submit(m_pluginAPI->getDSPEngineMessageQueue());
	}
}