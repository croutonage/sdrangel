///////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 Edouard Griffiths, F4EXB                                   //
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

#ifndef SDRBASE_DEVICE_DEVICESOURCEAPI_H_
#define SDRBASE_DEVICE_DEVICESOURCEAPI_H_

#include <QObject>
#include <QString>

#include "dsp/dspdevicesourceengine.h"

#include "util/export.h"

class MainWindow;
class GLSpectrum;
class ChannelWindow;
class BasebandSampleSink;
class ThreadedBasebandSampleSink;
class DeviceSampleSource;
class MessageQueue;
class ChannelMarker;
class QWidget;
class PluginGUI;
class PluginAPI;
class Preset;
class DeviceSinkAPI;

class SDRANGEL_API DeviceSourceAPI : public QObject {
    Q_OBJECT

public:
    // Device engine stuff
    void addSink(BasebandSampleSink* sink);       //!< Add a sample sink to device engine
    void removeSink(BasebandSampleSink* sink);    //!< Remove a sample sink from device engine
    void addThreadedSink(ThreadedBasebandSampleSink* sink);     //!< Add a sample sink that will run on its own thread to device engine
    void removeThreadedSink(ThreadedBasebandSampleSink* sink);  //!< Remove a sample sink that runs on its own thread from device engine
    void setSource(DeviceSampleSource* source); //!< Set device engine sample source type
    bool initAcquisition();               //!< Initialize device engine acquisition sequence
    bool startAcquisition();              //!< Start device engine acquisition sequence
    void stopAcquisition();               //!< Stop device engine acquisition sequence
    DSPDeviceSourceEngine::State state() const; //!< device engine state
    QString errorMessage();               //!< Return the current device engine error message
    uint getDeviceUID() const;            //!< Return the current device engine unique ID
    MessageQueue *getDeviceInputMessageQueue();
    MessageQueue *getDeviceOutputMessageQueue();
    void configureCorrections(bool dcOffsetCorrection, bool iqImbalanceCorrection); //!< Configure current device engine DSP corrections

    // device related stuff
    GLSpectrum *getSpectrum();                           //!< Direct spectrum getter
    void addChannelMarker(ChannelMarker* channelMarker); //!< Add channel marker to spectrum
    ChannelWindow *getChannelWindow();                   //!< Direct channel window getter
    void addRollupWidget(QWidget *widget);               //!< Add rollup widget to channel window
    void setInputGUI(QWidget* inputGUI, const QString& sourceDisplayName);

    void setHardwareId(const QString& id);
    void setSampleSourceId(const QString& id);
    void setSampleSourceSerial(const QString& serial);
    void setSampleSourceSequence(int sequence);
    void setSampleSourcePluginGUI(PluginGUI *gui);

    const QString& getHardwareId() const { return m_hardwareId; }
    const QString& getSampleSourceId() const { return m_sampleSourceId; }
    const QString& getSampleSourceSerial() const { return m_sampleSourceSerial; }
    int getSampleSourceSequence() const { return m_sampleSourceSequence; }

    void registerChannelInstance(const QString& channelName, PluginGUI* pluginGUI);
    void removeChannelInstance(PluginGUI* pluginGUI);

    void freeAll();

    void loadSourceSettings(const Preset* preset);
    void saveSourceSettings(Preset* preset);
    void loadChannelSettings(const Preset* preset, PluginAPI *pluginAPI);
    void saveChannelSettings(Preset* preset);

    MainWindow *getMainWindow() { return m_mainWindow; }
    DSPDeviceSourceEngine *getDeviceSourceEngine() { return m_deviceSourceEngine; }

    const std::vector<DeviceSourceAPI*>& getSourceBuddies() const { return m_sourceBuddies; }
    const std::vector<DeviceSinkAPI*>& getSinkBuddies() const { return m_sinkBuddies; }
    void addSourceBuddy(DeviceSourceAPI* buddy);
    void addSinkBuddy(DeviceSinkAPI* buddy);
    void removeSourceBuddy(DeviceSourceAPI* buddy);
    void removeSinkBuddy(DeviceSinkAPI* buddy);
    void clearBuddiesLists();
    void *getBuddySharedPtr() const { return m_buddySharedPtr; }
    void setBuddySharedPtr(void *ptr) { m_buddySharedPtr = ptr; }

protected:
    struct ChannelInstanceRegistration
    {
        QString m_channelName;
        PluginGUI* m_gui;

        ChannelInstanceRegistration() :
            m_channelName(),
            m_gui(NULL)
        { }

        ChannelInstanceRegistration(const QString& channelName, PluginGUI* pluginGUI) :
            m_channelName(channelName),
            m_gui(pluginGUI)
        { }

        bool operator<(const ChannelInstanceRegistration& other) const;
    };

    typedef QList<ChannelInstanceRegistration> ChannelInstanceRegistrations;

    DeviceSourceAPI(MainWindow *mainWindow,
            int deviceTabIndex,
            DSPDeviceSourceEngine *deviceSourceEngine,
            GLSpectrum *glSpectrum,
            ChannelWindow *channelWindow);
    ~DeviceSourceAPI();

    void renameChannelInstances();

    MainWindow *m_mainWindow;
    int m_deviceTabIndex;
    DSPDeviceSourceEngine *m_deviceSourceEngine;
    GLSpectrum *m_spectrum;
    ChannelWindow *m_channelWindow;

    QString m_hardwareId;
    QString m_sampleSourceId;
    QString m_sampleSourceSerial;
    int m_sampleSourceSequence;
    PluginGUI* m_sampleSourcePluginGUI;

    ChannelInstanceRegistrations m_channelInstanceRegistrations;

    std::vector<DeviceSourceAPI*> m_sourceBuddies; //!< Device source APIs referencing the same physical device
    std::vector<DeviceSinkAPI*> m_sinkBuddies;     //!< Device sink APIs referencing the same physical device
    void *m_buddySharedPtr;

    friend class MainWindow;
    friend class DeviceSinkAPI;
};


#endif /* SDRBASE_DEVICE_DEVICESOURCEAPI_H_ */
