#include "SelectionPad.h"

#include <QDebug>
#include <QPainter>
#include <QStyleOption>
#include "core/subchannel/SubchannelManager.h"


SelectionPad::SelectionPad(int id, QSize *widgetSize, QWidget *parent)
    : QWidget(parent)
{
    /** pad colors **/
    m_padId = id;
    m_padNr = m_padId%4;

    switch (m_padNr)
    {
        case 0:  //red 255,0,0
            m_padOnColor  = "{background-color: rgba(255,0,0,100%);}";
            m_padOffColor = "{background-color: rgba(255,0,0,15%);}";
            break;
        case 1: //blue 0,0,255
            m_padOnColor  = "{background-color: rgba(0,0,255,100%);}";
            m_padOffColor = "{background-color: rgba(0,0,255,15%);}";
            break;
        case 2: //yellow 255,255,0
            m_padOnColor  = "{background-color: rgba(255,255,0,100%);}";
            m_padOffColor = "{background-color: rgba(255,255,0,15%);}";
            break;
        case 3: //magenta 255,0,255
            m_padOnColor  = "{background-color: rgba(255,0,255,100%);}";
            m_padOffColor = "{background-color: rgba(255,0,255,15%);}";
            break;
        default:
            break;
    }

    /** sizes **/
    m_widgetSize = new QSize;
    m_widgetSize->setHeight(widgetSize->height());
    m_widgetSize->setWidth(widgetSize->width());

    int wP = m_widgetSize->width()/20;      // width pad
    int hP = m_widgetSize->height()/20;     // height pad
    int mS = 2;
    int lS = 12;



    m_selectionFrame = new QLabel(this);
    m_selectionFrame->setFixedSize( m_widgetSize->width(),  m_widgetSize->height());
    m_selectionFrame->setObjectName("subchannelSelectionFrame");
    m_selectionFrame->setStyleSheet("QLabel#subchannelSelectionFrame {background-color: rgba(0,0,0,0%); border: 2px solid rgba(100,100,100,0%);}");

    /**  layer 0 - widget **/
    this->setObjectName("pad_widget");
    this->setFixedSize(*m_widgetSize);


    m_volumeWidget = new VolumeWidget(this);
    m_volumeWidget->move(wP*19, hP*2);


    /** layer 1 - info tiles **/
    m_sampleNameLeft = new QLabel(this);
    m_sampleNameLeft->setObjectName("sampleNameLeft");
    m_sampleNameLeft->setStyleSheet("QLabel#sampleNameLeft {background-color: rgba(127,127,127, 0%); color: rgb(180,180,180); }");
    //m_sampleNameLeft->move( (m_widgetSize->width()/20)*2, (m_widgetSize->height()/5) * 1.4 );
    m_sampleNameLeft->move( (m_widgetSize->width()/20)*2.5, (m_widgetSize->height()/5) * 2.4 );
    m_sampleNameLeft->setAlignment(Qt::AlignLeft);
    m_sampleNameLeft->setFixedWidth((m_widgetSize->width()/20)*16);

#if 0
    m_sampleNameRight = new QLabel(this);
    m_sampleNameRight->setObjectName("sampleNameRight");
    m_sampleNameRight->setStyleSheet("QLabel#sampleNameRight {background-color: rgba(127,127,127, 0%); color: rgb(180,180,180); }");
    m_sampleNameRight->move( (m_widgetSize->width()/20)*2, (m_widgetSize->height()/5) * 2.0 );
    m_sampleNameRight->setAlignment(Qt::AlignRight);
    m_sampleNameRight->setFixedWidth((m_widgetSize->width()/20)*18);
#endif

    m_sampleTime = new QLabel(this);
    m_sampleTime->setObjectName("sampleTime");
    m_sampleTime->setStyleSheet("QLabel#sampleTime {background-color: rgba(127,127,127, 0%); color: rgb(180,180,180); }");
    m_sampleTime->move( (m_widgetSize->width()/20)*2, (m_widgetSize->height()/5) * 4 );
    m_sampleTime->setAlignment(Qt::AlignCenter);

    m_sampleSteps = new QLabel(this);
    m_sampleSteps->setObjectName("sampleSteps");
    m_sampleSteps->setStyleSheet("QLabel#sampleSteps {background-color: rgba(127,127,127, 0%); color: rgb(180,180,180); }");
    m_sampleSteps->move( ((m_widgetSize->width()/20) * 10), (m_widgetSize->height()/5) * 4 );
    m_sampleSteps->setAlignment(Qt::AlignRight);


#if 1 // subchannel color pads
    m_subChColor = new QLabel(this);
    m_subChColor->setObjectName("color");
    m_subChColor->setFixedSize(14,14);
    m_subChColor->move( (m_widgetSize->width()/20)*9.9 , m_widgetSize->height()/5 * 4);
    setPadToDeselectionColor();
#endif

    /** tiles **/
    m_hasSteps = new QLabel(this);
    m_hasSteps->setObjectName("tile_hasStep");
    m_hasSteps->setStyleSheet("QLabel#tile_hasStep {background-color: rgb(127,127,127);}");
    m_hasSteps->setFixedSize( wP*4, hP*4 );
    m_hasSteps->move( wP*mS, hP*mS   );
    m_hasSteps->setText("[]");
    m_hasSteps->setAlignment(Qt::AlignCenter);

    m_isPlaying = new QLabel(this);
    m_isPlaying->setObjectName( "tile_isPlaying");
    m_isPlaying->setFixedSize( wP*10, hP*4);
    m_isPlaying->move( wP*8, hP*mS);
    m_isPlaying->setText(">");
    m_isPlaying->setAlignment(Qt::AlignCenter);


    /** layer 2 - button color */
    m_buttonPadColor = new QLabel(this);
    m_buttonPadColor->setObjectName("pad_buttonColor");
    m_buttonPadColor->setFixedSize(*m_widgetSize);


    /** layer 3 - button **/
    m_buttonPad = new QPushButton(this);
    m_buttonPad->setObjectName("pad_button");
    m_buttonPad->setFixedSize(*m_widgetSize);
    //m_buttonPad->setText(QString::number(id));



    /* internal connections */
    connect(m_buttonPad, SIGNAL(pressed()),  this, SLOT(m_slot_padPressed()) );
}

SelectionPad::~SelectionPad()
{

}



/** tiles **/
void SelectionPad::setHasSteps()
{
    //m_hasSteps->setStyleSheet("QLabel#tile_hasStep {background-color: rgb(250,250,250);}");
    m_hasSteps->setStyleSheet("QLabel#tile_hasStep" + m_padOffColor);
}

void SelectionPad::clearHasSteps()
{
    m_hasSteps->setStyleSheet("QLabel#tile_hasStep {background-color: rgb(127,127,127);}");
    //m_hasSteps->setStyleSheet(m_padOffColor);
}

void SelectionPad::setHasSample()
{
    //m_hasSample->setStyleSheet("QLabel#tile_hasSample {background-color: rgb(250,250,250);}");
}

void SelectionPad::clearHasSample()
{
    //m_hasSample->setStyleSheet("QLabel#tile_hasSample {background-color: rgb(127,127,127);}");
}

void SelectionPad::setIsPlaying()
{
    //m_isPlaying->setStyleSheet("QLabel#tile_isPlaying {background-color: rgb(250,250,250);}");
    m_isPlaying->setStyleSheet("QLabel#tile_isPlaying" + m_padOnColor);
}

void SelectionPad::clearIsPlaying()
{
    m_isPlaying->setStyleSheet("QLabel#tile_isPlaying {background-color: rgb(127,127,127);}");
}


void SelectionPad::setSampleParameters(QString name, float time, float steps)
{
#if 0
    m_sampleNameRight->setText(name);
    m_sampleNameRight->adjustSize();
#endif
    m_sampleNameLeft->setText(name);
    m_sampleNameLeft->adjustSize();
    m_sampleTime->setText( QString::number(time/1000, 'f', 2) + " s" );
    m_sampleTime->adjustSize();
    m_sampleSteps->setText( "[" + QString::number(steps, 'f', 2) + "]");
    m_sampleSteps->adjustSize();
}

void SelectionPad::clearSampleName()
{
#if 0
    m_sampleNameRight->setText(" ");
#endif
    m_sampleTime->setText(" ");
}

void SelectionPad::setSampleVolume()
{
    m_volumeValue = subchannelManager().getSubchannelVolume(m_padId);
    m_volumeWidget->setVolume(m_volumeValue);
}


void SelectionPad::m_slot_padPressed()
{
    emit signal_subchPadPressed(m_padId);
}

void SelectionPad::setPadToSelectionColor(void)
{
    //m_subChColor->setStyleSheet("QLabel#color" + m_padOnColor);
    //m_selectionFrame->setStyleSheet("QLabel#subchannelSelectionFrame {background-color: rgba(0,0,0,0%); border: 2px solid rgba(180,180,180,100%);}");
    //m_selectionFrame->setStyleSheet("QLabel#subchannelSelectionFrame {background-color: rgba(0,0,0,0%); border: 2px solid rgba(180,180,180,100%);}");

    switch (m_padNr)
    {
        case 0:  //red 255,0,0
            m_selectionFrame->setStyleSheet("QLabel#subchannelSelectionFrame {background-color: rgba(0,0,0,0%); border: 2px solid rgba(255,  0,  0,25%);}");
            break;
        case 1: //blue 0,0,255
            m_selectionFrame->setStyleSheet("QLabel#subchannelSelectionFrame {background-color: rgba(0,0,0,0%); border: 2px solid rgba(  0,  0,255,25%);}");
            break;
        case 2: //yellow 255,255,0
            m_selectionFrame->setStyleSheet("QLabel#subchannelSelectionFrame {background-color: rgba(0,0,0,0%); border: 2px solid rgba(255,255,  0,25%);}");
            break;
        case 3: //magenta 255,0,255
            m_selectionFrame->setStyleSheet("QLabel#subchannelSelectionFrame {background-color: rgba(0,0,0,0%); border: 2px solid rgba(255,  0,255,25%);}");
            break;
        default:
            break;
    }

}

void SelectionPad::setPadToDeselectionColor(void)
{
    //m_subChColor->setStyleSheet("QLabel#color" + m_padOffColor);
    m_selectionFrame->setStyleSheet("QLabel#subchannelSelectionFrame {background-color: rgba(0,0,0,0%); border: 2px solid rgba(0,0,0,0%);}");
}





void SelectionPad::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QStyleOption myOption;
    myOption.initFrom(this);
    QPainter myPainter(this);
    style()->drawPrimitive( QStyle::PE_Widget, &myOption, &myPainter, this);
}
