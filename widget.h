#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QIODevice>
#include <QAudioInput>
#include <fftw3.h>
#include <cmath>

inline double rmod(fftw_complex val)
{
    return sqrt( val[0]*val[0] + val[1]*val[1]);
}

class AudioReadDevice : public QIODevice
{
    Q_OBJECT
    friend class Widget;
public:
    AudioReadDevice(QObject *parent) : QIODevice(parent){
        dp = (fftw_complex *) fftw_malloc(sizeof(fftw_complex)*1024);
    }

protected:
    qint64 readData(char *data, qint64 maxlen){ return 0; }
    qint64 writeData(const char *data, qint64 len);

private:
    unsigned char buffer[1024];
    fftw_complex *dp;

signals:
    void toDisplay(fftw_complex *dp);
};

namespace Ui {
    class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    void paintEvent(QPaintEvent *);

private slots:
    void on_pushButton_clicked();
    void display(fftw_complex *dp);

private:
    bool reading;
    AudioReadDevice *audioReadDevice;
    QAudioInput *audioInput;
    Ui::Widget *ui;
    QPointF line[1024];
    QRect dpArea;
    fftw_plan p;
    fftw_complex *rr;
    double yfact;
};

#endif // WIDGET_H
