#include "../../channelrx/demodnfm/nfmdemodgui.h"

#include <device/devicesourceapi.h>
#include <dsp/downchannelizer.h>
#include <QDockWidget>
#include <QMainWindow>
#include <QDebug>

#include "../../../sdrbase/dsp/threadedbasebandsamplesink.h"
#include "ui_nfmdemodgui.h"
#include "dsp/nullsink.h"
#include "plugin/pluginapi.h"
#include "util/simpleserializer.h"
#include "util/db.h"
#include "gui/basicchannelsettingswidget.h"
#include "dsp/dspengine.h"
#include "mainwindow.h"
#include "../../channelrx/demodnfm/nfmdemod.h"

const QString NFMDemodGUI::m_channelID = "de.maintech.sdrangelove.channel.nfm";

const int NFMDemodGUI::m_rfBW[] = {
	5000, 6250, 8330, 10000, 12500, 15000, 20000, 25000, 40000
};
const int NFMDemodGUI::m_fmDev[] = { // corresponding FM deviations
    1000, 1500, 2000, 2000,  2000,  2500,  3000,  3500,  5000
};
const int NFMDemodGUI::m_nbRfBW = 9;

NFMDemodGUI* NFMDemodGUI::create(PluginAPI* pluginAPI, DeviceSourceAPI *deviceAPI)
{
	NFMDemodGUI* gui = new NFMDemodGUI(pluginAPI, deviceAPI);
	return gui;
}

void NFMDemodGUI::destroy()
{
	delete this;
}

void NFMDemodGUI::setName(const QString& name)
{
	setObjectName(name);
}

QString NFMDemodGUI::getName() const
{
	return objectName();
}

qint64 NFMDemodGUI::getCenterFrequency() const
{
	return m_channelMarker.getCenterFrequency();
}

void NFMDemodGUI::setCenterFrequency(qint64 centerFrequency)
{
	m_channelMarker.setCenterFrequency(centerFrequency);
	applySettings();
}

void NFMDemodGUI::resetToDefaults()
{
	blockApplySettings(true);

	ui->rfBW->setCurrentIndex(4);
	ui->afBW->setValue(3);
	ui->volume->setValue(20);
	ui->squelchGate->setValue(5);
	ui->squelch->setValue(-40);
	ui->deltaFrequency->setValue(0);
	ui->ctcssOn->setChecked(false);
	ui->audioMute->setChecked(false);

	blockApplySettings(false);
	applySettings();
}

QByteArray NFMDemodGUI::serialize() const
{
	SimpleSerializer s(1);
	s.writeS32(1, m_channelMarker.getCenterFrequency());
	s.writeS32(2, ui->rfBW->currentIndex());
	s.writeS32(3, ui->afBW->value());
	s.writeS32(4, ui->volume->value());
	s.writeS32(5, ui->squelch->value());
	s.writeU32(7, m_channelMarker.getColor().rgb());
	s.writeS32(8, ui->ctcss->currentIndex());
	s.writeBool(9, ui->ctcssOn->isChecked());
	s.writeBool(10, ui->audioMute->isChecked());
	s.writeS32(11, ui->squelchGate->value());
	s.writeBool(12, ui->deltaSquelch->isChecked());
	return s.final();
}

bool NFMDemodGUI::deserialize(const QByteArray& data)
{
	SimpleDeserializer d(data);

	if (!d.isValid())
	{
		resetToDefaults();
		return false;
	}

	if (d.getVersion() == 1)
	{
		QByteArray bytetmp;
		quint32 u32tmp;
		qint32 tmp;
		bool boolTmp;

		blockApplySettings(true);
		m_channelMarker.blockSignals(true);

		d.readS32(1, &tmp, 0);
		m_channelMarker.setCenterFrequency(tmp);
		d.readS32(2, &tmp, 4);
		ui->rfBW->setCurrentIndex(tmp);
		d.readS32(3, &tmp, 3);
		ui->afBW->setValue(tmp);
		d.readS32(4, &tmp, 20);
		ui->volume->setValue(tmp);
		d.readS32(5, &tmp, -40);
		ui->squelch->setValue(tmp);

		if(d.readU32(7, &u32tmp))
		{
			m_channelMarker.setColor(u32tmp);
		}

		d.readS32(8, &tmp, 0);
		ui->ctcss->setCurrentIndex(tmp);
		d.readBool(9, &boolTmp, false);
		ui->ctcssOn->setChecked(boolTmp);
		d.readBool(10, &boolTmp, false);
		ui->audioMute->setChecked(boolTmp);
		d.readS32(11, &tmp, 5);
		ui->squelchGate->setValue(tmp);
        d.readBool(12, &boolTmp, false);
        ui->deltaSquelch->setChecked(boolTmp);

		blockApplySettings(false);
		m_channelMarker.blockSignals(false);

		applySettings();
		return true;
	}
	else
	{
		resetToDefaults();
		return false;
	}
}

bool NFMDemodGUI::handleMessage(const Message& message)
{
	return false;
}

void NFMDemodGUI::viewChanged()
{
	applySettings();
}

void NFMDemodGUI::on_deltaMinus_toggled(bool minus)
{
	int deltaFrequency = m_channelMarker.getCenterFrequency();
	bool minusDelta = (deltaFrequency < 0);

	if (minus ^ minusDelta) // sign change
	{
		m_channelMarker.setCenterFrequency(-deltaFrequency);
	}
}

void NFMDemodGUI::on_deltaFrequency_changed(quint64 value)
{
	if (ui->deltaMinus->isChecked())
	{
		m_channelMarker.setCenterFrequency(-value);
	}
	else
	{
		m_channelMarker.setCenterFrequency(value);
	}
}

void NFMDemodGUI::on_rfBW_currentIndexChanged(int index)
{
	qDebug() << "NFMDemodGUI::on_rfBW_currentIndexChanged" << index;
	//ui->rfBWText->setText(QString("%1 k").arg(m_rfBW[value] / 1000.0));
	m_channelMarker.setBandwidth(m_rfBW[index]);
	applySettings();
}

void NFMDemodGUI::on_afBW_valueChanged(int value)
{
	ui->afBWText->setText(QString("%1 k").arg(value));
	applySettings();
}

void NFMDemodGUI::on_volume_valueChanged(int value)
{
	ui->volumeText->setText(QString("%1").arg(value / 10.0, 0, 'f', 1));
	applySettings();
}

void NFMDemodGUI::on_squelchGate_valueChanged(int value)
{
	applySettings();
}

void NFMDemodGUI::on_deltaSquelch_toggled(bool checked)
{
    if (ui->deltaSquelch->isChecked())
    {
        ui->squelchText->setText(QString("%1").arg((-ui->squelch->value()) / 10.0, 0, 'f', 1));
        ui->squelchText->setToolTip(tr("Squelch deviation threshold (%)"));
    }
    else
    {
        ui->squelchText->setText(QString("%1").arg(ui->squelch->value() / 10.0, 0, 'f', 1));
        ui->squelchText->setToolTip(tr("Squelch power threshold (dB)"));
    }
    applySettings();
}

void NFMDemodGUI::on_squelch_valueChanged(int value)
{
    if (ui->deltaSquelch->isChecked())
    {
        ui->squelchText->setText(QString("%1").arg(-value / 10.0, 0, 'f', 1));
        ui->squelchText->setToolTip(tr("Squelch deviation threshold (%)"));
    }
    else
    {
        ui->squelchText->setText(QString("%1").arg(value / 10.0, 0, 'f', 1));
        ui->squelchText->setToolTip(tr("Squelch power threshold (dB)"));
    }
	applySettings();
}

void NFMDemodGUI::on_ctcssOn_toggled(bool checked)
{
	m_ctcssOn = checked;
	applySettings();
}

void NFMDemodGUI::on_audioMute_toggled(bool checked)
{
	m_audioMute = checked;
	applySettings();
}

void NFMDemodGUI::on_ctcss_currentIndexChanged(int index)
{
	if (m_nfmDemod != 0)
	{
		m_nfmDemod->setSelectedCtcssIndex(index);
	}
}

void NFMDemodGUI::onWidgetRolled(QWidget* widget, bool rollDown)
{
	/*
	if((widget == ui->spectrumContainer) && (m_nfmDemod != NULL))
		m_nfmDemod->setSpectrum(m_threadedSampleSink->getMessageQueue(), rollDown);
	*/
}

void NFMDemodGUI::onMenuDoubleClicked()
{
	if (!m_basicSettingsShown)
	{
		m_basicSettingsShown = true;
		BasicChannelSettingsWidget* bcsw = new BasicChannelSettingsWidget(&m_channelMarker, this);
		bcsw->show();
	}
}

NFMDemodGUI::NFMDemodGUI(PluginAPI* pluginAPI, DeviceSourceAPI *deviceAPI, QWidget* parent) :
	RollupWidget(parent),
	ui(new Ui::NFMDemodGUI),
	m_pluginAPI(pluginAPI),
	m_deviceAPI(deviceAPI),
	m_channelMarker(this),
	m_basicSettingsShown(false),
	m_doApplySettings(true),
	m_squelchOpen(false)
{
	ui->setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose, true);

	blockApplySettings(true);
	ui->rfBW->clear();
	for (int i = 0; i < m_nbRfBW; i++) {
		ui->rfBW->addItem(QString("%1").arg(m_rfBW[i] / 1000.0, 0, 'f', 2));
	}
	blockApplySettings(false);

	connect(this, SIGNAL(widgetRolled(QWidget*,bool)), this, SLOT(onWidgetRolled(QWidget*,bool)));
	connect(this, SIGNAL(menuDoubleClickEvent()), this, SLOT(onMenuDoubleClicked()));

	m_nfmDemod = new NFMDemod();
	m_nfmDemod->registerGUI(this);

	connect(&m_pluginAPI->getMainWindow()->getMasterTimer(), SIGNAL(timeout()), this, SLOT(tick()));

	int ctcss_nbTones;
	const Real *ctcss_tones = m_nfmDemod->getCtcssToneSet(ctcss_nbTones);

	ui->ctcss->addItem("--");

	for (int i=0; i<ctcss_nbTones; i++)
	{
		ui->ctcss->addItem(QString("%1").arg(ctcss_tones[i]));
	}

	ui->audioMute->setStyleSheet("QToolButton { background:rgb(79,79,79); }"); // squelch closed

	ui->deltaFrequency->setColorMapper(ColorMapper(ColorMapper::ReverseGold));
    ui->channelPowerMeter->setColorTheme(LevelMeterSignalDB::ColorGreenAndBlue);

	m_channelizer = new DownChannelizer(m_nfmDemod);
	m_threadedChannelizer = new ThreadedBasebandSampleSink(m_channelizer, this);
	m_deviceAPI->addThreadedSink(m_threadedChannelizer);

	//m_channelMarker = new ChannelMarker(this);
	m_channelMarker.setColor(Qt::red);
	m_channelMarker.setBandwidth(12500);
	m_channelMarker.setCenterFrequency(0);
	m_channelMarker.setVisible(true);

	connect(&m_channelMarker, SIGNAL(changed()), this, SLOT(viewChanged()));

	m_deviceAPI->registerChannelInstance(m_channelID, this);
	m_deviceAPI->addChannelMarker(&m_channelMarker);
	m_deviceAPI->addRollupWidget(this);

	QChar delta = QChar(0x94, 0x03);
	ui->deltaSquelch->setText(delta);

	applySettings();
}

NFMDemodGUI::~NFMDemodGUI()
{
    m_deviceAPI->removeChannelInstance(this);
    m_deviceAPI->removeThreadedSink(m_threadedChannelizer);
	delete m_threadedChannelizer;
	delete m_channelizer;
	delete m_nfmDemod;
	//delete m_channelMarker;
	delete ui;
}

void NFMDemodGUI::applySettings()
{
	if (m_doApplySettings)
	{
		qDebug() << "NFMDemodGUI::applySettings";

		setTitleColor(m_channelMarker.getColor());

		m_channelizer->configure(m_channelizer->getInputMessageQueue(),
			48000,
			m_channelMarker.getCenterFrequency());

		ui->deltaFrequency->setValue(abs(m_channelMarker.getCenterFrequency()));
		ui->deltaMinus->setChecked(m_channelMarker.getCenterFrequency() < 0);
		ui->squelchGateText->setText(QString("%1").arg(ui->squelchGate->value() * 10.0f, 0, 'f', 0));

		m_nfmDemod->configure(m_nfmDemod->getInputMessageQueue(),
			m_rfBW[ui->rfBW->currentIndex()],
			ui->afBW->value() * 1000.0f,
			m_fmDev[ui->rfBW->currentIndex()],
			ui->volume->value() / 10.0f,
			ui->squelchGate->value(), // in 10ths of ms
			ui->deltaSquelch->isChecked(),
			ui->squelch->value(), // -1000 -> 0
			ui->ctcssOn->isChecked(),
			ui->audioMute->isChecked());
	}
}

void NFMDemodGUI::leaveEvent(QEvent*)
{
	blockApplySettings(true);
	m_channelMarker.setHighlighted(false);
	blockApplySettings(false);
}

void NFMDemodGUI::enterEvent(QEvent*)
{
	blockApplySettings(true);
	m_channelMarker.setHighlighted(true);
	blockApplySettings(false);
}

void NFMDemodGUI::setCtcssFreq(Real ctcssFreq)
{
	if (ctcssFreq == 0)
	{
		ui->ctcssText->setText("--");
	}
	else
	{
		ui->ctcssText->setText(QString("%1").arg(ctcssFreq));
	}
}

void NFMDemodGUI::blockApplySettings(bool block)
{
	m_doApplySettings = !block;
}

void NFMDemodGUI::tick()
{
    Real magsqAvg, magsqPeak;
    int nbMagsqSamples;
    m_nfmDemod->getMagSqLevels(magsqAvg, magsqPeak, nbMagsqSamples);
    Real powDbAvg = CalcDb::dbPower(magsqAvg);
    Real powDbPeak = CalcDb::dbPower(magsqPeak);

    ui->channelPowerMeter->levelChanged(
            (100.0f + powDbAvg) / 100.0f,
            (100.0f + powDbPeak) / 100.0f,
            nbMagsqSamples);

    ui->channelPower->setText(QString::number(powDbAvg, 'f', 1));

    bool squelchOpen = m_nfmDemod->getSquelchOpen();

	if (squelchOpen != m_squelchOpen)
	{
		if (squelchOpen) {
			ui->audioMute->setStyleSheet("QToolButton { background-color : green; }");
		} else {
			ui->audioMute->setStyleSheet("QToolButton { background:rgb(79,79,79); }");
		}

        m_squelchOpen = squelchOpen;
	}
}
