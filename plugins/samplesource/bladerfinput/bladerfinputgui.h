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

#ifndef INCLUDE_BLADERFINPUTGUI_H
#define INCLUDE_BLADERFINPUTGUI_H

#include <QTimer>
#include "plugin/plugingui.h"

#include "../bladerfinput/bladerfinput.h"

class DeviceSourceAPI;
class FileRecord;

namespace Ui {
	class BladerfInputGui;
}

class BladerfInputGui : public QWidget, public PluginGUI {
	Q_OBJECT

public:
	explicit BladerfInputGui(DeviceSourceAPI *deviceAPI, QWidget* parent = NULL);
	virtual ~BladerfInputGui();
	void destroy();

	void setName(const QString& name);
	QString getName() const;

	void resetToDefaults();
	virtual qint64 getCenterFrequency() const;
	virtual void setCenterFrequency(qint64 centerFrequency);
	QByteArray serialize() const;
	bool deserialize(const QByteArray& data);
	virtual bool handleMessage(const Message& message);

private:
	Ui::BladerfInputGui* ui;

	DeviceSourceAPI* m_deviceAPI;
	BladeRFInputSettings m_settings;
	QTimer m_updateTimer;
	QTimer m_statusTimer;
	std::vector<int> m_gains;
	DeviceSampleSource* m_sampleSource;
    FileRecord *m_fileSink; //!< File sink to record device I/Q output
    int m_sampleRate;
    quint64 m_deviceCenterFrequency; //!< Center frequency in device
	int m_lastEngineState;

	void displaySettings();
	void sendSettings();
	unsigned int getXb200Index(bool xb_200, bladerf_xb200_path xb200Path, bladerf_xb200_filter xb200Filter);
	void updateSampleRateAndFrequency();

private slots:
    void handleDSPMessages();
	void on_centerFrequency_changed(quint64 value);
    void on_sampleRate_changed(quint64 value);
	void on_dcOffset_toggled(bool checked);
	void on_iqImbalance_toggled(bool checked);
	void on_bandwidth_currentIndexChanged(int index);
	void on_decim_currentIndexChanged(int index);
	void on_lna_currentIndexChanged(int index);
	void on_vga1_valueChanged(int value);
	void on_vga2_valueChanged(int value);
	void on_xb200_currentIndexChanged(int index);
	void on_fcPos_currentIndexChanged(int index);
	void on_startStop_toggled(bool checked);
    void on_record_toggled(bool checked);
	void updateHardware();
	void updateStatus();
};

#endif // INCLUDE_BLADERFINPUTGUI_H
