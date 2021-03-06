#include "Hwui_01.h"

#include "core/midi/MidiNames.h"
#include "Settings.h"
#include "MosaikTypes.h"

#include <QDebug>




Hwui_01::Hwui_01(QString hwPort)
{
	m_hwPort = hwPort;

	m_midiOut = new MidiOut(m_hwPort);

	m_midiIn  = new MidiIn(m_hwPort);
	connect(m_midiIn, SIGNAL(signal_midiMsgReceived(quint8*)), this, SLOT(slot_midiMsgReceived(quint8*)) );
	m_midiIn->start();

	m_mute = false;
	m_shiftMainVol = true;
	m_shiftSubVol = false;
	m_shiftPan = false;
	m_shiftBpm = false;
	m_shiftHpVol = false;
	m_tempHpVol = 0.0;
}


Hwui_01::~Hwui_01()
{
	m_midiIn->stop();
	delete m_midiIn;
	delete m_midiOut;
}


void Hwui_01::slot_midiMsgReceived(quint8 *data)
{
	//qDebug() <<Q_FUNC_INFO <<data[0] <<data[1] <<data[2];

	quint8 midiBuffer[3] = {};
	midiBuffer[0] = data[0];    // midi ch, status
	midiBuffer[1] = data[1];    // note/id
	midiBuffer[2] = data[2];    // value

	if( midiBuffer[0] == 0xB0 ) // CC
	{
		if( midiBuffer[1] == 5 )
		{
			if( m_shiftMainVol )
			{
				emit signal_mainVolume( ((float) (midiBuffer[2] - 64)) / 100 ); //works
			}
			else if( m_shiftSubVol )
			{
				emit signal_erpChanged(4, (data[2] - 64));	// subch vol
			}
			else if( m_shiftPan )
			{
				emit signal_currentPan(((float) (data[2] - 64)) / 100 );	// works
			}
			else if( m_shiftBpm )
			{
				emit signal_bpmChanged( (float) (data[2] - 64) );
			}
			else if( m_shiftHpVol )
			{
				m_tempHpVol = m_tempHpVol + ((float) (data[2] - 64)) * 0.01;
				emit signal_headphoneVolume( m_tempHpVol );
			}
		}
	}
	else if( midiBuffer[0] == 0x90 )
	{
		switch( midiBuffer[1] )
		{
#if 0
			case  0:	emit signal_button00Pressed(); break;
			case  1:	emit signal_button01Pressed(); break;
			case  2:	emit signal_button02Pressed(); break;
			case  3:	emit signal_button03Pressed(); break;
#endif
			case  4:
			{
			qDebug() <<Q_FUNC_INFO <<"mail vol";
				m_shiftMainVol = true;
				m_shiftSubVol = false;
				m_shiftPan = false;
				m_shiftBpm = false;
				m_shiftHpVol = false;
				break;
			}
			case  5:
			{
				qDebug() <<Q_FUNC_INFO <<"bpm";
				m_shiftBpm = true;
				m_shiftMainVol = false;
				m_shiftSubVol = false;
				m_shiftPan = false;
				m_shiftHpVol = false;
				break;
			}
			case  1:
			{
				qDebug() <<Q_FUNC_INFO <<"sub vol";
				m_shiftSubVol = true;
				m_shiftMainVol = false;
				m_shiftPan = false;
				m_shiftBpm = false;
				m_shiftHpVol = false;
				break;
			}
			case  3:
			{
				qDebug() <<Q_FUNC_INFO <<"pan";
				m_shiftPan = true;
				m_shiftMainVol = false;
				m_shiftSubVol = false;
				m_shiftBpm = false;
				m_shiftHpVol = false;
				break;
			}
			case  6:
			{
				qDebug() <<Q_FUNC_INFO <<"hp vol";
				m_shiftHpVol = true;
				m_shiftMainVol = false;
				m_shiftSubVol = false;
				m_shiftPan = false;
				m_shiftBpm = false;
				break;
			}
			case  7:	emit signal_button07Pressed(); break;
			case  8:
			{
				m_mute = !m_mute;
				emit signal_button08Pressed(m_mute);
				break;
			}
			case  9:	emit signal_button09Pressed(); break;
			case 10:	emit signal_button10Pressed(); break;
			case 11:	emit signal_button11Pressed(); break;
		}
	}
}

