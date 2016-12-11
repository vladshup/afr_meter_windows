#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QThread>
#include <QtCore/qmath.h>




QT_USE_NAMESPACE

namespace Ui {
class MainWindow;
}



class Sleeper : public QThread
{
public:
    static void usleep(unsigned long usecs){QThread::usleep(usecs);}
    static void msleep(unsigned long msecs){QThread::msleep(msecs);}
    static void sleep(unsigned long secs){QThread::sleep(secs);}
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void choisePort(int);
    void setFreq();
    void setN();
    void setMode();

private:
    Ui::MainWindow *ui;
    QSerialPortInfo *info;
    QSerialPort *serial;
    void MainWindow::openSerialPort();
    void MainWindow::closeSerialPort();
    void MainWindow::readData();
    void MainWindow::writeData(QString data);
    double Fmin;
    double Fmax;
    double Fc;
    double BW;
    double deltaF;
    unsigned int N;


};

#endif // MAINWINDOW_H
