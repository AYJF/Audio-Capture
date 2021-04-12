#include <QDebug>
#include <QPainter>
#include "widget.h"
#include "ui_widget.h"

qint64 AudioReadDevice::writeData(const char *data, qint64 len)
{

    static int i=0;
    int p = (i+len > 1024) ? (1024-i) : len;
    memcpy(buffer + i, data, p); i+=p; i&=1023;
    memcpy(buffer + i, data + p, len-p); i+= (len-p); i&=1023;


    fftw_complex *iter = dp;
    for (p=0; p < 1024; ++p, ++iter){
        *iter[0] = buffer[(i+p)&1023];
        *iter[1] = 0.0;
    }

    emit toDisplay(dp);
    return len;
}



Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    reading = false;
    dpArea.setRect(ui->pushButton->width()+2, ui->pushButton->height()+2, 1024, 256);
    audioReadDevice = new AudioReadDevice(this);

    rr = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*1024);
    p = fftw_plan_dft_1d(1024, audioReadDevice->dp, rr,FFTW_FORWARD,FFTW_MEASURE);


}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_clicked()
{
    QAudioFormat m_format;
    m_format.setFrequency(8000);
    m_format.setChannels(1);
    m_format.setSampleSize(8);
    m_format.setSampleType(QAudioFormat::SignedInt);
    m_format.setByteOrder(QAudioFormat::LittleEndian);
    m_format.setCodec("audio/pcm");

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultInputDevice());
    if (!info.isFormatSupported(m_format)) {
        qWarning() << "Default format not supported - trying to use nearest";
        m_format = info.nearestFormat(m_format);
    }

      audioInput = new QAudioInput(m_format, this);
      connect(audioReadDevice, SIGNAL(toDisplay(fftw_complex*)), SLOT(display(fftw_complex*)));
      audioReadDevice->open(QIODevice::WriteOnly);
      audioInput->start(audioReadDevice);

      reading = true;
}

void Widget::paintEvent(QPaintEvent *ev)
{
    Q_UNUSED (ev)
    if (!reading) return;

    QPainter painter(this);
    //painter.drawRect(dpArea);
    painter.translate(dpArea.x(), dpArea.y() + dpArea.height());
    painter.scale(1, 1/*-1024.0/yfact*/);
    painter.drawPolyline(line+1, 511);
}


void Widget::display(fftw_complex *dp)
{
    fftw_execute(p);
    yfact = 0.0;
    double tmp;
    for (int i=0; i < 512; ++i){
        line[i].setX(i);
        line[i].setY( tmp = -rmod(rr[i]));
        if (yfact > tmp) yfact = tmp;
    }
    update();
}
