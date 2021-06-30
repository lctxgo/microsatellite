#ifndef MAINWINDOW_H
#define MAINWINDOW_H



#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QList>
#include <QTimer>
#include <chartswindow.h>
#include <QtCharts>
QT_CHARTS_USE_NAMESPACE

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void btnScanComInfo();
    void btnOpenCom();
    void btnCloseCom();
    void handleReadyRead();
    void btnSend();
    //以下两个函数实现字符串转为16进制
    char convertHexChar(char ch);
    QByteArray QStringToHex(QString str);
    void btnSendRST();
    void btnSendTime();
    void btnSendSend();
    void btnSendTH1();
    void btnSendTH2();
    void btnSendTH3();

    void handleSerialTimeOut();
    void handleChartTimeOut();
    void handleCheckbox();
    void openChartWindow();

    int dotNum = 0;

    QSplineSeries *getSeries();



private:
    Ui::MainWindow *ui;
    QSerialPort *serial = new QSerialPort(this);
    QTimer myTimer;
    QTimer chartTimer;
    QTimer serialTimer;
    chartsWindow chartWindow;
    QByteArray receiveData;

    QSplineSeries *series = new QSplineSeries();
    QChart *theChart;
    QValueAxis *axisX;
    QValueAxis *axisY;

    QDateTime datetime;
    QString timestr;
    QFile data;
    bool titlenum;
};

#endif // MAINWINDOW_H
