#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    serial = new QSerialPort(this);
    connect(serial, &QSerialPort::readyRead, this, &MainWindow::readData);

    ui->setupUi(this);

    //start settings
    ui->lineEdit->setText("1000");//Fmin
    ui->lineEdit_2->setText("30000");//Fmax
    ui->startButton->setText("Set");
    for (int i = 5; i <= 8; i++)//Set N frequensy points for xAxis
    {
        int item = qPow(2,i);
        ui->comboBox_2->addItem(QString::number(item), QString::number(i-8));
    }
    ui->comboBox_3->addItem("Fmin/Fmax", QString::number(0));
    ui->comboBox_3->addItem("Fc/BW", QString::number(1));

    //Finde available system serial ports
    Q_FOREACH (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
        {
        ui->comboBox->addItem(serialPortInfo.portName(), serialPortInfo.serialNumber());
        }

    connect(ui->comboBox, SIGNAL (currentIndexChanged(int)), this, SLOT (choisePort(int)));//choise serial
    connect(ui->comboBox_2, SIGNAL (currentIndexChanged(int)), this, SLOT (setN()));//set N
    connect(ui->comboBox_3, SIGNAL (currentIndexChanged(int)), this, SLOT (setMode()));//set Mode
    connect(ui->startButton, SIGNAL (clicked(bool)), this, SLOT (setFreq()));//"Start" button set freq's


    ui->textEdit->setText("Choise COM port");

    // создаем график и добавляем  данные:
    ui->widget->addGraph();
    ui->widget->addGraph();
    // задаем имена осей координат
    ui->widget->xAxis->setLabel("kHz");
    ui->widget->yAxis->setLabel("dB");
    //расчет частот
    Fmin = ui->lineEdit->displayText().toInt();
    Fmax = ui->lineEdit_2->displayText().toInt();
    N = 32;//N points
    deltaF = Fmax - Fmin;
    // задаем размеры осей
    ui->widget->xAxis->setRange(Fmin, Fmax);
    ui->widget->yAxis->setRange(-60, 10);

}


//Open COM-port
void MainWindow::openSerialPort()
{



    serial->setPortName(ui->comboBox->currentText());

    if (serial->open(QIODevice::ReadWrite))
    {
        serial->setBaudRate(QSerialPort::Baud19200);
        serial->setDataBits(QSerialPort::Data8);
        serial->setParity(QSerialPort::NoParity);
        serial->setStopBits(QSerialPort::OneStop);
        serial->setFlowControl(QSerialPort::NoFlowControl);
    } else {
            ui->textEdit->setText(serial->errorString());
            }
}


void MainWindow::closeSerialPort()
{
    if (serial->isOpen())
        serial->close();
}

void MainWindow::readData()
{
    if (serial->bytesAvailable()>=N )
    {
        QByteArray data  = serial->readAll();

        QVector<double> x(N), y(N);

            for (unsigned int i=0; i < N; i++)
            {
              x[i] = Fmin + i*(deltaF/(N-1));
              y[i] = ((data[i]+128))/4 - 60;//char to byte and scale to -60dB

            }
            ui->widget->graph(0)->setData(x, y);
            ui->widget->xAxis->rescale();
            ui->widget->replot();
        //Sleeper::msleep(10);
        serial->clear();
    }
}

void MainWindow::writeData(QString data)
{

data.append('\n');
if (serial->isWritable() && serial->isOpen())
{


        serial->write((char*)data.toLocal8Bit().constData(), data.size());
        //Sleeper::msleep(100);
qDebug() << "Error is " << serial->errorString();

if ( serial->bytesToWrite() > 0)
{

     serial->flush();

}
Sleeper::msleep(300);
}
/*
    serial->write((char*)data.toLocal8Bit().constData());
    if ( serial->bytesToWrite() > 0)
    {
         serial->flush();
    }
*/
}


void MainWindow::choisePort(int)
{
    if (serial->isOpen()) {MainWindow::closeSerialPort();}
    ui->textEdit->setText(ui->comboBox->currentText());
    MainWindow::openSerialPort();
}

void MainWindow::setFreq()
{
QString buf1 = ui->lineEdit->displayText();
QString buf2 = ui->lineEdit_2->displayText();

//Mode Fc/BW
if(ui->comboBox_3->currentText() == "Fc/BW")
{
    //read from interface
    Fc = buf1.toInt();
    BW = buf2.toInt();
    //check bounds
    if (BW > Fc*2){BW = Fc*2;}
    //calculate Fmin,Fmax
    Fmin = Fc - BW/2;
    Fmax = Fc + BW/2;
    //set bounds
    if (Fmin < 9) {Fmin = 9;}
    if (Fmax > 220000) {Fmax = 220000;}
    //abs and rounding
    Fc = round(fabs(Fc));
    BW = round(fabs(BW));
    Fmin = round(fabs(Fmin));
    Fmax = round(fabs(Fmax));
    //change BW in interface
    ui->lineEdit_2->setText(QString::number(BW));
}

//Mode Fmin/Fmax
if (ui->comboBox_3->currentText() == "Fmin/Fmax"){
    //read from interface
    Fmin = buf1.toInt();
    Fmax = buf2.toInt();
    //set bounds
    if (Fmin < 9) {Fmin = 9; ui->lineEdit->setText("9");}
    if (Fmax > 220000) {Fmax = 220000;ui->lineEdit_2->setText("220000");}
    //abs and rounding
    Fmin = round(fabs(Fmin));
    Fmax = round(fabs(Fmax));
}





//write Fmin, Fmax to serial port
buf1 = "f1"+QString::number(Fmin);
MainWindow::writeData(buf1);
buf2 = "f2"+QString::number(Fmax);
MainWindow::writeData(buf2);
//Change xAxis
ui->widget->xAxis->setRange(Fmin, Fmax);
ui->widget->replot();
//Count deltaF
deltaF = Fmax - Fmin;
}

void MainWindow::setN()
{
    QString buf = ui->comboBox_2->currentText();
    if (buf == "256") buf = "255";
    buf = "n"+buf;
    MainWindow::writeData(buf);
    buf = ui->comboBox_2->currentText();
    if (buf == "256") buf = "255";
    N = buf.toInt();
    //ui->textEdit->setText(ui->comboBox_2->currentText());
}


void MainWindow::setMode()
{
ui->textEdit->setText(ui->comboBox_3->currentText());
if (ui->comboBox_3->currentText() == "Fmin/Fmax"){ui->label->setText("Fmin(kHz)");ui->label_2->setText("Fmax(kHz)");}
if (ui->comboBox_3->currentText() == "Fc/BW"){ui->label->setText("Fc(kHz)");ui->label_2->setText("BW(kHz)");}
}

MainWindow::~MainWindow()
{
    MainWindow::closeSerialPort();
    delete ui;
}
