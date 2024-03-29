#include "MidiManager.h"

#include <QDebug>
#include <QString>
#include <QObject>

#include "Settings.h"
#include "core/midi/MidiInfo.h"
#include "core/midi/devices/MosaikMini.h"
#include "core/midi/devices/SparkLe.h"
#include "core/midi/devices/VirtualMidiDevice.h"



MidiManager::MidiManager()
{
    qDebug() <<Q_FUNC_INFO <<"Init";
    m_isConnected = false;
    //connectFavouriteDevice();
}

MidiManager::~MidiManager()
{
    qDebug() <<"~" <<Q_FUNC_INFO;
}




void MidiManager::connectFavouriteDevice(void)
{
    qDebug() <<Q_FUNC_INFO;

    midiInfo().rescanMidiPorts();


    QList<QString> hwMidiNameList = midiInfo().getDeviceNameList();
    QList<QString> hwMidiPortList = midiInfo().getDevicePortList();
    QList<QString> favMidiDevList = settings().getFavouriteMidiDeviceList();

    int midiHwDevice = -1;

    for(int fav = 0; (fav < favMidiDevList.size()) && (midiHwDevice == -1); fav++ )
    {
        //qDebug() <<"fav" <<fav;
        for(int dev = 0; (dev < hwMidiNameList.size()) && (midiHwDevice == -1); dev++ )
        {
            //qDebug() <<"dev" <<dev;
            if( hwMidiNameList.at(dev) == favMidiDevList.at(fav) )
            {
                midiHwDevice = dev;
                qDebug() <<Q_FUNC_INFO <<"midiHwDevice: " <<midiHwDevice;
            }
        }
    }

    if( m_isConnected )
        delete m_midiDevice;


    if( midiHwDevice == -1 )
    {
        qDebug() <<Q_FUNC_INFO <<"Connected to virtual";
        settings().setConnectedMidiDeviceName("virtual");
        settings().setConnectedMidiDevicePort("virtual");
        m_midiDevice = new VirtualMidiDevice();
    }
    else
    {
        qDebug() <<Q_FUNC_INFO <<"Favourite device is:" <<hwMidiNameList.at(midiHwDevice);
        settings().setConnectedMidiDeviceName(hwMidiNameList.at(midiHwDevice));
        settings().setConnectedMidiDevicePort(hwMidiPortList.at(midiHwDevice));

        if( hwMidiNameList.at(midiHwDevice) == "MOSAIKmini" )
        {
            m_midiDevice = new MosaikMini();

            /** app **/
            connect( m_midiDevice, SIGNAL(signal_functionLeftButton00Pressed()),         m_parent, SLOT(slot_appToggleFullScreen()) );
            connect( m_midiDevice, SIGNAL(signal_functionLeftButton01Pressed()),         m_parent, SLOT(slot_appExit()) );

            /** selection **/
            connect( m_midiDevice, SIGNAL(signal_functionSelectSubchannelRelative(int)), m_parent, SLOT(slot_selectionSetCurrentSubchannelRelative(int)) );
            connect( m_midiDevice, SIGNAL(signal_selectChannel(int)),                    m_parent, SLOT(slot_selectionSetCurrentChannel(int) ) );

            /** browser **/
            connect( m_midiDevice, SIGNAL(signal_encChanged(int)),                       m_parent, SLOT(slot_browserChangeCursorPosition(int)) );
            connect( m_midiDevice, SIGNAL(signal_menuNavigation(int)),                   m_parent, SLOT(slot_browserChangeCursorPosition(int)) );
            connect( m_midiDevice, SIGNAL(signal_menuEncoderPushed()),                   m_parent, SLOT(slot_browserToggleFolderExpansion()) );
            connect( m_midiDevice, SIGNAL(signal_setPathId(int)),                        m_parent, SLOT(slot_browserChangePathId(int)) );

            /** prelisten **/
            connect( m_midiDevice, SIGNAL(signal_prelistenBrowserSample()),              m_parent, SLOT(slot_browserSelectedSampleToPrelisten()) );
            connect( m_midiDevice, SIGNAL(signal_prelistenSubchannelSample()),           m_parent, SLOT(slot_prelistenSubchannelSample()) );

            /** sequencer/pattern **/
            connect( m_midiDevice, SIGNAL(signal_stepButtonPressed(int)),                m_parent, SLOT(slot_stepButtonPressed(int)) );
            connect( m_midiDevice, SIGNAL(signal_functionRightButton04Pressed()),        m_parent, SLOT(slot_patternClearAll()) );
            connect( m_midiDevice, SIGNAL(signal_functionRightButton06Pressed()),        m_parent, SLOT(slot_patternClearCurrentChannel()) );
            connect( m_midiDevice, SIGNAL(signal_functionRightButton07Pressed()),        m_parent, SLOT(slot_patternClearCurrentSubchannel()) );

            /** main parameter **/
            connect( m_midiDevice, SIGNAL(signal_bpmChanged(float)),                     m_parent, SLOT(slot_globalChangeBpmRelative(float)) );
            connect( m_midiDevice, SIGNAL(signal_mainVolume(float)),                     m_parent, SLOT(slot_globalMainVolume(float)) );
            connect( m_midiDevice, SIGNAL(signal_headphoneVolume(float)),                m_parent, SLOT(slot_globalPreVolume(float)) );

            /** sample **/
            connect( m_midiDevice, SIGNAL(signal_functionRightButton00Pressed()),        m_parent, SLOT(slot_sampleUnloadAll() ) );
            connect( m_midiDevice, SIGNAL(signal_functionRightButton02Pressed()),        m_parent, SLOT(slot_sampleUnloadCurrentChannel() ) );
            connect( m_midiDevice, SIGNAL(signal_functionRightButton03Pressed()),        m_parent, SLOT(slot_sampleUnloadCurrentSubchannel() ) );
            connect( m_midiDevice, SIGNAL(signal_loadSample()),                          m_parent, SLOT(slot_sampleLoadToCurrentSubchannel() ) );

            /** subchannel parameter **/
            connect( m_midiDevice, SIGNAL(signal_currentPan(float)),                     m_parent, SLOT(slot_parameterPan(float)) );
            connect( m_midiDevice, SIGNAL(signal_currentSubchannelToPre(bool)),          m_parent, SLOT(slot_parameterCurrentSubToPre(bool)) );
            connect( m_midiDevice, SIGNAL(signal_muteAndSolo(bool)),                     m_parent, SLOT(slot_parameterMuteAndSolo(bool)) );
            connect( m_midiDevice, SIGNAL(signal_unmuteAll()),                           m_parent, SLOT(slot_parameterUnmuteAll()) );
            connect( m_midiDevice, SIGNAL(signal_lastMute()),                            m_parent, SLOT(slot_parameterSelectLastMutes()) );

            //connect( m_midiDevice, SIGNAL(signal_encChanged(float)),                     m_parent, SLOT(slot_subchannelChangeVolume(float)) );
            connect( m_midiDevice, SIGNAL(signal_erpChanged(quint8,qint8)),              m_parent, SLOT(slot_erpChanged(quint8,qint8)) );
            connect( m_midiDevice, SIGNAL(signal_selectPageSubchannel()),                m_parent, SLOT(slot_uiSetToPageSubchannel()) );
            connect( m_midiDevice, SIGNAL(signal_selectPageBrowser()),                   m_parent, SLOT(slot_uiSetToPageBrowser()) );


        }
        else if( hwMidiNameList.at(midiHwDevice) == "MOSAIK" )
        {
            //m_midiDevice = new Mosaik;
        }
        else if( hwMidiNameList.at(midiHwDevice) == "SparkLE" )
        {
            m_midiDevice = new SparkLe();
            connect( m_midiDevice, SIGNAL(signal_stepButtonPressed(int)), m_parent, SLOT(slot_stepButtonPressed(int)) );
            connect( m_midiDevice, SIGNAL(signal_erpChanged(quint8,qint8)), m_parent, SLOT(slot_erpChanged(quint8,qint8)) );

            connect( m_midiDevice, SIGNAL(signal_bpmChanged(float)),           m_parent, SLOT(slot_globalChangeBpmRelative(float)) );
            //connect( m_midiDevice, SIGNAL(signal_mainVolume(quint8)),          m_parent, SLOT(slot_setMainVolume(quint8)) );
            connect( m_midiDevice, SIGNAL(signal_currentSubchannelVol(float)), m_parent, SLOT(slot_parameterChangeCurrentSubchVolume(float)) );

            connect( m_midiDevice, SIGNAL(signal_currentSubchannelPanRel(float)), m_parent, SLOT(slot_parameterPan(float)) );
        }
        else if( hwMidiNameList.at(midiHwDevice) == "UNTZtrument" )
        {
            //m_midiDevice = new Untztrument;
        }
        else
        {
            qDebug() <<Q_FUNC_INFO <<"Device not in List!";
        }
    }

    m_isConnected = true;
}



void MidiManager::setStepLed(int i)
{
    m_midiDevice->setStepLed(i);
}

void MidiManager::setParentWidget(MosaikMiniApp *parent)
{
    m_parent = parent;
}

void MidiManager::slot_erpChanged(quint8 id, qint8 val)
{
    qDebug() <<Q_FUNC_INFO <<"Midi Received:" <<"id:" <<id <<"val:" <<val;
    m_parent->slot_erpChanged(id, val);
}

void MidiManager::sendData(const QByteArray &data)
{
    qDebug() << Q_FUNC_INFO << data.size();
    m_midiDevice->sendRawData(data);
}

void MidiManager::setChannelPattern()
{
    m_midiDevice->setChannelPattern();
}

void MidiManager::refreshPatternView()
{
    m_midiDevice->refreshSequencer();
}

void MidiManager::refreshSubchannelSelection()
{
    m_midiDevice->refreshSubchannelSelection();
}

void MidiManager::setStepsequencerLed(int stepLedId)
{
    m_midiDevice->setStepsequencerLed(stepLedId);
}

void MidiManager::setMainVolume(float volume)
{
    int vol = 127 - (127 * volume);
    qDebug() <<Q_FUNC_INFO <<"vol" <<vol;
    m_midiDevice->setMainVolume(vol);
}

void MidiManager::resetHardware()
{
    m_midiDevice->resetHardware();
}

void MidiManager::subToPreLed(bool state)
{
    m_midiDevice->subToPreLed(state);
}
