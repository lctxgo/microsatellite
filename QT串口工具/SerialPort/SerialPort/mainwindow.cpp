#include <QtCharts>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QString>
#include <QDebug>
#include <QList>
#include <QLabel>
#include <QTextBrowser>
#include <QComboBox>
#include <QMessageBox>
#include <QTimer>
#include <QByteArray>
#include <QCheckBox>
#include <QAction>
#include <QMenuBar>
#include <QDateTimeAxis>
#include <QFile>
#include <QDateTime>



Q_DECLARE_METATYPE(QSerialPort::DataBits)//Q_DECLARE_METATYPE宏进行声明
Q_DECLARE_METATYPE(QSerialPort::Parity)
Q_DECLARE_METATYPE(QSerialPort::StopBits)

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("GroveStreet");
    ui->textBrowserRec->append("鸡你太美");
    setWindowIcon(QIcon("64icon.ico"));
    //获取所有的串口信息

    const QList<QSerialPortInfo> ComInfos = QSerialPortInfo::availablePorts();
    //设置串口选择
    for(const QSerialPortInfo &ComInfo : ComInfos)
    {
        ui->comboBoxCom->addItem(ComInfo.portName()+"("+ComInfo.description()+")",ComInfo.portName());
    }



    //设置波特率
    ui->comboBoxBaud->addItem("115200",QSerialPort::Baud115200);
    ui->comboBoxBaud->addItem("9600",QSerialPort::Baud9600);        //捆绑数据
    ui->comboBoxBaud->addItem("19200",QSerialPort::Baud19200);
    ui->comboBoxBaud->addItem("38400",QSerialPort::Baud38400);

    //设置数据位
    ui->comboBoxDatabits->addItem("8",QSerialPort::Data8);
    ui->comboBoxDatabits->addItem("5",QSerialPort::Data5);
    ui->comboBoxDatabits->addItem("6",QSerialPort::Data6);
    ui->comboBoxDatabits->addItem("7",QSerialPort::Data7);

    //设置奇偶检验位
    ui->comboBoxParity->addItem("奇检验",QSerialPort::OddParity);
    ui->comboBoxParity->addItem("偶检验",QSerialPort::EvenParity);
    ui->comboBoxParity->addItem("无",QSerialPort::NoParity);

    //设置停止位
    ui->comboBoxStopbits->addItem("1",QSerialPort::OneStop);
    ui->comboBoxStopbits->addItem("1.5",QSerialPort::OneAndHalfStop);
    ui->comboBoxStopbits->addItem("2",QSerialPort::TwoStop);

    if(serial->isOpen())
    {

    }
    ui->textRST->setText("1160C000000500F0AABBAA4B");
    ui->textTime->setText("1160C0000009FFFF000F11112222CCC3");
    ui->textSend->setText("1000C0000005AAAAAAAA0000");
    ui->textTH1->setText("1160C000000900F100001111222233C2");
    ui->textTH2->setText("1160C000000900F200001111222233C1");
    ui->textTH3->setText("1160C000000900F400001111222233C7");
    connect(ui->btnRST,&QPushButton::clicked,this,&MainWindow::btnSendRST);
    connect(ui->btnTime,&QPushButton::clicked,this,&MainWindow::btnSendTime);
    connect(ui->btnSend,&QPushButton::clicked,this,&MainWindow::btnSendSend);
    connect(ui->btnTH1,&QPushButton::clicked,this,&MainWindow::btnSendTH1);
    connect(ui->btnTH2,&QPushButton::clicked,this,&MainWindow::btnSendTH2);
    connect(ui->btnTH3,&QPushButton::clicked,this,&MainWindow::btnSendTH3);



    connect(ui->ButtonScanCom,&QPushButton::clicked,this,&MainWindow::btnScanComInfo);
    connect(ui->ButtonOpenCom,&QPushButton::clicked,this,&MainWindow::btnOpenCom);
    connect(ui->ButtonCloseCom,&QPushButton::clicked,this,&MainWindow::btnCloseCom);
    connect(ui->ButtonSend,&QPushButton::clicked,this,&MainWindow::btnSend);
    connect(ui->ButtonClearRec,&QPushButton::clicked,[=](){ui->textBrowserRec->clear();});
    connect(ui->ButtonClearSend,&QPushButton::clicked,[=](){ui->textEditSend->clear();});

    connect(serial,&QSerialPort::readyRead,this,&MainWindow::handleReadyRead);

    connect(&myTimer,&QTimer::timeout,this,&MainWindow::btnSendSend);
    connect(&chartTimer,&QTimer::timeout,this,&MainWindow::handleChartTimeOut);
    connect(ui->checkBoxTimer,&QCheckBox::stateChanged,this,&MainWindow::handleCheckbox);
    connect(ui->ChartAction,&QAction::triggered,this,&MainWindow::openChartWindow);

    connect(&serialTimer,&QTimer::timeout,this,&MainWindow::handleSerialTimeOut);

    titlenum = 1;


    chartTimer.setInterval(100);



    theChart = new QChart();
    theChart->setTitle("探测器照度");
    theChart->legend()->hide();
//    theChart->setAnimationOptions(QChart::AllAnimations);

//    QPen green(Qt::red);
//    green.setWidth(3);
//    series->setPen(green);
    axisX = new QValueAxis();
    axisY = new QValueAxis();
    axisX->setTickCount(20);
    series->append(0,0);
    theChart->addSeries(series);
    theChart->addAxis(axisX,Qt::AlignBottom);
    theChart->addAxis(axisY,Qt::AlignLeft);
    series->attachAxis(axisX);
    series->attachAxis(axisY);
    axisX->setRange(0, 65536);
    axisY->setRange(0, 65536);

    this->chartWindow.getChartView()->setChart(theChart);

    this->chartWindow.getChartView()->setRenderHint(QPainter::Antialiasing);

}
void MainWindow::btnScanComInfo()
{
    ui->comboBoxCom->clear();
    const QList<QSerialPortInfo> ComInfos = QSerialPortInfo::availablePorts();
    ui->textBrowserRec->append("当前" + QString::number(ComInfos.count(),4) + "个设备可用。\n");
    //将串口信息拆分组合成一个QString，这里用到了c++11 for range的用法
    for(const QSerialPortInfo &ComInfo : ComInfos)
    {
        QString s = "串口名：" + ComInfo.portName() + "\n" +
                    "串口号：" + ComInfo.serialNumber() + "\n" +
                    "设备ID：" + (ComInfo.hasProductIdentifier()? QString::number(ComInfo.hasProductIdentifier(),16):"NULL") + "\n" +
                    "厂商ID：" + (ComInfo.hasVendorIdentifier()?QString::number(ComInfo.hasVendorIdentifier(),16):"NULL") + "\n" +
                    "制造商：" + ComInfo.manufacturer() + "\n" +
                    "设备状态：" + (ComInfo.isBusy()?"设备正忙":"设备空闲") + "\n" +
                    "描述：" + ComInfo.description() + "\n";
        ui->textBrowserRec->append(s);

        ui->comboBoxCom->addItem(ComInfo.portName()+"("+ComInfo.description()+")",ComInfo.portName());
    }

}

void MainWindow::btnOpenCom()
{
    serial->setPortName(ui->comboBoxCom->currentData().toString());
    serial->setBaudRate(ui->comboBoxBaud->currentData().toInt());
    serial->setDataBits((ui->comboBoxDatabits->currentData()).value<QSerialPort::DataBits>());
    serial->setParity((ui->comboBoxParity->currentData()).value<QSerialPort::Parity>());
    serial->setStopBits((ui->comboBoxStopbits->currentData()).value<QSerialPort::StopBits>());
    qDebug() << "当前打开的串口是" << ui->comboBoxCom->currentText()
             << "\n波特率为" << ui->comboBoxBaud->currentData().toInt()
             << "\n数据位数为" << ui->comboBoxDatabits->currentText()
             << "\n停止位为" << ui->comboBoxStopbits->currentText()
             << "\n奇偶检验方式为" << ui->comboBoxParity->currentText();
    if(serial->open(QIODevice::ReadWrite))
    {

    }
    else
    {
        QMessageBox::critical(this,"错误","打开串口失败");

    }

}
void MainWindow::btnSend()
{
    QByteArray captureSendStr = MainWindow::QStringToHex(ui->textEditSend->toPlainText());
    qDebug() << captureSendStr;
    if(serial->write(captureSendStr) == -1)
        QMessageBox::critical(this,"错误","???\n先打开串口!");
}
void MainWindow::btnCloseCom()
{
    if(serial->isOpen())
    {
        serial->close();
    }
}
void MainWindow::handleSerialTimeOut()
{
    qDebug() << receiveData.length();
    ui->textBrowser->setText(QString::number((receiveData.mid(0,2).toHex().toInt(nullptr,16))%4096,16).toUpper());


    ui->textBrowser_2->setText(receiveData.mid(0,6).toHex().toUpper());     //包头
    ui->textBrowser_3->setText(receiveData.mid(6,1).toHex().toUpper());     //帧头
    ui->textBrowser_4->setText(receiveData.mid(7,2).toHex().toUpper());     //复位
    ui->textBrowser_5->setText(receiveData.mid(9,6).toHex().toUpper());    //传感器1
    ui->textBrowser_6->setText(receiveData.mid(15,6).toHex().toUpper());    //传感器2
    ui->textBrowser_7->setText(receiveData.mid(21,6).toHex().toUpper());    //传感器3
    ui->textBrowser_8->setText(receiveData.mid(27,6).toHex().toUpper());    //时间码


//    ui->textBrowser_9->setText(receiveData.mid(33,6).toHex().toUpper());    //探测器1方向
//    ui->textBrowser_10->setText(receiveData.mid(39,6).toHex().toUpper());   //探测器2
//    ui->textBrowser_11->setText(receiveData.mid(45,6).toHex().toUpper());   //探测器3
//    ui->textBrowser_12->setText(receiveData.mid(51,6).toHex().toUpper());   //探测器4
//    ui->textBrowser_13->setText(receiveData.mid(57,6).toHex().toUpper());   //探测器5
    ui->textBrowser_27->setText(receiveData.mid(33,6).toHex().toUpper());    //探测器1方向
    ui->textBrowser_28->setText(receiveData.mid(39,6).toHex().toUpper());   //探测器2
    ui->textBrowser_29->setText(receiveData.mid(45,6).toHex().toUpper());   //探测器3
    ui->textBrowser_30->setText(receiveData.mid(51,6).toHex().toUpper());   //探测器4
    ui->textBrowser_31->setText(receiveData.mid(57,6).toHex().toUpper());   //探测器5


    ui->textBrowser_14->setText("2b:"+QString::number((receiveData.mid(63,1).toHex().toInt(nullptr,16))/64,2));
    ui->textBrowser_15->setText("2b:"+QString::number((receiveData.mid(63,1).toHex().toInt(nullptr,16))/16%4,2));
    ui->textBrowser_16->setText("2b:"+QString::number(((receiveData.mid(63,1).toHex().toInt(nullptr,16))%16)/4,2));
    ui->textBrowser_17->setText("2b:"+QString::number((receiveData.mid(63,1).toHex().toInt(nullptr,16))%4,2));
    ui->textBrowser_18->setText("2b:"+QString::number((receiveData.mid(64,1).toHex().toInt(nullptr,16))/64,2));
    ui->textBrowser_19->setText("6b:"+QString::number((receiveData.mid(64,1).toHex().toInt(nullptr,16))%64,2));//告警标志位
    ui->textBrowser_20->setText(receiveData.mid(65,2).toHex().toUpper());
    ui->textBrowser_21->setText("4b:"+QString::number((receiveData.mid(67,1).toHex().toInt(nullptr,16))/16,2));
    ui->textBrowser_22->setText("4b:"+QString::number((receiveData.mid(67,1).toHex().toInt(nullptr,16))%16,2));
    ui->textBrowser_23->setText("4b:"+QString::number((receiveData.mid(68,1).toHex().toInt(nullptr,16))/16,2));
    ui->textBrowser_24->setText("4b:"+QString::number((receiveData.mid(68,1).toHex().toInt(nullptr,16))%16,2));
    ui->textBrowser_25->setText("6b:"+QString::number((receiveData.mid(69,1).toHex().toInt(nullptr,16))/4,2));
    ui->textBrowser_26->setText("2b:"+QString::number((receiveData.mid(69,1).toHex().toInt(nullptr,16))%4,2));//方向标志位

    //照度
//    ui->textBrowser_27->setText(receiveData.mid(70,2).toHex().toUpper());
//    ui->textBrowser_28->setText(receiveData.mid(72,2).toHex().toUpper());
//    ui->textBrowser_29->setText(receiveData.mid(74,2).toHex().toUpper());
//    ui->textBrowser_30->setText(receiveData.mid(76,2).toHex().toUpper());
//    ui->textBrowser_31->setText(receiveData.mid(78,2).toHex().toUpper());
//    ui->textBrowser_27->setText(QString::number((receiveData.mid(70,3).toHex().toInt(nullptr,16))/64,2));
//    ui->textBrowser_28->setText(QString::number((receiveData.mid(72,3).toHex().toInt(nullptr,16))/16%65536,2));
//    ui->textBrowser_29->setText(QString::number((receiveData.mid(74,3).toHex().toInt(nullptr,16))/4%16384,2));
//    ui->textBrowser_30->setText(QString::number((receiveData.mid(76,3).toHex().toInt(nullptr,16))%262144,2));
//    ui->textBrowser_31->setText(receiveData.mid(79,2).toHex().toUpper());
    ui->textBrowser_9->setText(QString::number((receiveData.mid(70,3).toHex().toInt(nullptr,16))/64,16).toUpper()+" 十进制："+QString::number((receiveData.mid(70,3).toHex().toInt(nullptr,16))/64,10));
    ui->textBrowser_10->setText(QString::number((receiveData.mid(72,3).toHex().toInt(nullptr,16))/16%262144,16).toUpper()+" 十进制："+QString::number((receiveData.mid(72,3).toHex().toInt(nullptr,16))/16%262144,10));
    ui->textBrowser_11->setText(QString::number((receiveData.mid(74,3).toHex().toInt(nullptr,16))/4%262144,16).toUpper()+" 十进制："+QString::number((receiveData.mid(74,3).toHex().toInt(nullptr,16))/4%262144,10));
    ui->textBrowser_12->setText(QString::number((receiveData.mid(76,3).toHex().toInt(nullptr,16))%262144,16).toUpper()+" 十进制："+QString::number((receiveData.mid(76,3).toHex().toInt(nullptr,16))%262144,10));
    ui->textBrowser_13->setText(QString(receiveData.mid(79,2).toHex().toUpper()) +" 十进制："+QString::number(receiveData.mid(79,2).toHex().toInt(nullptr,16),10));
    //温度
    ui->textBrowser_32->setText(QString::number((receiveData.mid(81,2).toHex().toInt(nullptr,16))/16,16).toUpper());
    ui->textBrowser_33->setText(QString::number((receiveData.mid(82,2).toHex().toInt(nullptr,16))%4096,16).toUpper());
    ui->textBrowser_34->setText(QString::number((receiveData.mid(84,2).toHex().toInt(nullptr,16))/16,16).toUpper());
    ui->textBrowser_35->setText(QString::number((receiveData.mid(85,2).toHex().toInt(nullptr,16))%4096,16).toUpper());
    ui->textBrowser_36->setText(QString::number((receiveData.mid(87,2).toHex().toInt(nullptr,16))/16,16).toUpper());
    ui->textBrowser_37->setText(QString::number((receiveData.mid(88,2).toHex().toInt(nullptr,16))%4096,16).toUpper());
    ui->textBrowser_38->setText(receiveData.mid(90,1).toHex().toUpper());
    //探测器1源码
//    ui->textBrowser_39->setText(QString::number((receiveData.mid(90,2).toHex().toInt(nullptr,16))/16,16).toUpper());
//    ui->textBrowser_40->setText(QString::number((receiveData.mid(91,2).toHex().toInt(nullptr,16))%4096,16).toUpper());
//    ui->textBrowser_41->setText(QString::number((receiveData.mid(93,2).toHex().toInt(nullptr,16))/16,16).toUpper());
//    ui->textBrowser_42->setText(QString::number((receiveData.mid(94,2).toHex().toInt(nullptr,16))%4096,16).toUpper());
    ui->textBrowser_39->setText(receiveData.mid(91,2).toHex().toUpper());
    ui->textBrowser_40->setText(receiveData.mid(93,2).toHex().toUpper());
    ui->textBrowser_41->setText(receiveData.mid(95,2).toHex().toUpper());
    ui->textBrowser_42->setText(receiveData.mid(97,2).toHex().toUpper());
    //探测器2源码
//    ui->textBrowser_43->setText(QString::number((receiveData.mid(96,2).toHex().toInt(nullptr,16))/16,16).toUpper());
//    ui->textBrowser_44->setText(QString::number((receiveData.mid(97,2).toHex().toInt(nullptr,16))%4096,16).toUpper());
//    ui->textBrowser_45->setText(QString::number((receiveData.mid(99,2).toHex().toInt(nullptr,16))/16,16).toUpper());
//    ui->textBrowser_46->setText(QString::number((receiveData.mid(100,2).toHex().toInt(nullptr,16))%4096,16).toUpper());
    ui->textBrowser_43->setText(receiveData.mid(99,2).toHex().toUpper());
    ui->textBrowser_44->setText(receiveData.mid(101,2).toHex().toUpper());
    ui->textBrowser_45->setText(receiveData.mid(103,2).toHex().toUpper());
    ui->textBrowser_46->setText(receiveData.mid(105,2).toHex().toUpper());
    //探测器3源码
//    ui->textBrowser_47->setText(QString::number((receiveData.mid(102,2).toHex().toInt(nullptr,16))/16,16).toUpper());
//    ui->textBrowser_48->setText(QString::number((receiveData.mid(103,2).toHex().toInt(nullptr,16))%4096,16).toUpper());
//    ui->textBrowser_49->setText(QString::number((receiveData.mid(105,2).toHex().toInt(nullptr,16))/16,16).toUpper());
//    ui->textBrowser_50->setText(QString::number((receiveData.mid(106,2).toHex().toInt(nullptr,16))%4096,16).toUpper());
    ui->textBrowser_47->setText(receiveData.mid(107,2).toHex().toUpper());
    ui->textBrowser_48->setText(receiveData.mid(109,2).toHex().toUpper());
    ui->textBrowser_49->setText(receiveData.mid(111,2).toHex().toUpper());
    ui->textBrowser_50->setText(receiveData.mid(113,2).toHex().toUpper());
    //探测器4源码
//    ui->textBrowser_51->setText(QString::number((receiveData.mid(108,2).toHex().toInt(nullptr,16))/16,16).toUpper());
//    ui->textBrowser_52->setText(QString::number((receiveData.mid(109,2).toHex().toInt(nullptr,16))%4096,16).toUpper());
//    ui->textBrowser_53->setText(QString::number((receiveData.mid(111,2).toHex().toInt(nullptr,16))/16,16).toUpper());
//    ui->textBrowser_54->setText(QString::number((receiveData.mid(112,2).toHex().toInt(nullptr,16))%4096,16).toUpper());
    ui->textBrowser_51->setText(receiveData.mid(115,2).toHex().toUpper());
    ui->textBrowser_52->setText(receiveData.mid(117,2).toHex().toUpper());
    ui->textBrowser_53->setText(receiveData.mid(119,2).toHex().toUpper());
    ui->textBrowser_54->setText(receiveData.mid(121,2).toHex().toUpper());
    //探测器5源码
    ui->textBrowser_55->setText(QString::number((receiveData.mid(123,2).toHex().toInt(nullptr,16))/16,16).toUpper());
    ui->textBrowser_56->setText(QString::number((receiveData.mid(124,2).toHex().toInt(nullptr,16))%4096,16).toUpper());
    ui->textBrowser_57->setText(QString::number((receiveData.mid(126,2).toHex().toInt(nullptr,16))/16,16).toUpper());
    ui->textBrowser_58->setText(QString::number((receiveData.mid(127,2).toHex().toInt(nullptr,16))%4096,16).toUpper());
    ui->textBrowser_59->setText(QString::number((receiveData.mid(129,2).toHex().toInt(nullptr,16))/16,16).toUpper());
    //多余4位0000
    //帧尾
    ui->textBrowser_60->setText(receiveData.mid(131,1).toHex().toUpper());
    if(ui->checkBox->isChecked())
    {
        if(titlenum == 1)
        {
            datetime = QDateTime::currentDateTime();
            timestr = datetime.toString("yyyyMMddHHmmss");
            data.setFileName(timestr + "TCQ_data" + ".csv");

            if(data.open(QFile::WriteOnly |QFile::Append ))      // 打开文件
            {
                QTextStream out(&data);    // 输入流
                out << " " << ","
                    << "TCQ1_I1" << "," << "TCQ1_I2" << "," << "TCQ1_I3" << "," << "TCQ1_I4" << "," << "TCQ1_total" << ","
                    << "TCQ2_I1" << "," << "TCQ2_I2" << "," << "TCQ2_I3" << "," << "TCQ2_I4" << "," << "TCQ2_total" << ","
                    << "TCQ3_I1" << "," << "TCQ3_I2" << "," << "TCQ3_I3" << "," << "TCQ3_I4" << "," << "TCQ3_total" << ","
                    << "TCQ4_I1" << "," << "TCQ4_I2" << "," << "TCQ4_I3" << "," << "TCQ4_I4" << "," << "TCQ4_total" << ","
                    << "TCQ5_I1" << "," << "TCQ5_I2" << "," << "TCQ5_I3" << "," << "TCQ5_I4" << "," << "TCQ5_I5" << "," << "TCQ5_total" << ","
                    << "TCQ1_Dir" << "," << "TCQ2_Dir" << ","<< "TCQ3_Dir" << ","<< "TCQ4_Dir" << "," << "TCQ5_Dir" << "," << "\n";
            }
            data.close();
            titlenum = 0;
        }
        // 以上两行用时间戳作为文件名
        if(data.open(QFile::WriteOnly |QFile::Append ))      // 打开文件
        {
            QTextStream out(&data);    // 输入流
            QString headFlag = MainWindow::ui->textSave->toPlainText();

            out << headFlag << ",";
            //qDebug() << QString::number(headFlag.toInt(nullptr,16),16);

            out << receiveData.mid(91,2).toHex().toUpper() << ","
                << receiveData.mid(93,2).toHex().toUpper() << ","
                << receiveData.mid(95,2).toHex().toUpper() << ","
                << receiveData.mid(97,2).toHex().toUpper() << ","
                << QString::number((receiveData.mid(70,3).toHex().toInt(nullptr,16))/64,2) << ","

                << receiveData.mid(99,2).toHex().toUpper() << ","
                << receiveData.mid(101,2).toHex().toUpper() << ","
                << receiveData.mid(103,2).toHex().toUpper() << ","
                << receiveData.mid(105,2).toHex().toUpper() << ","
                << QString::number((receiveData.mid(72,3).toHex().toInt(nullptr,16))/16%65536,2) << ","

                << receiveData.mid(107,2).toHex().toUpper() << ","
                << receiveData.mid(109,2).toHex().toUpper() << ","
                << receiveData.mid(111,2).toHex().toUpper() << ","
                << receiveData.mid(113,2).toHex().toUpper() << ","
                << QString::number((receiveData.mid(74,3).toHex().toInt(nullptr,16))/4%16384,2) << ","

                << receiveData.mid(115,2).toHex().toUpper() << ","
                << receiveData.mid(117,2).toHex().toUpper() << ","
                << receiveData.mid(119,2).toHex().toUpper() << ","
                << receiveData.mid(121,2).toHex().toUpper() << ","
                << QString::number((receiveData.mid(76,3).toHex().toInt(nullptr,16))%262144,2) << ","

                << QString::number((receiveData.mid(123,2).toHex().toInt(nullptr,16))/16,16).toUpper() << ","
                << QString::number((receiveData.mid(124,2).toHex().toInt(nullptr,16))%4096,16).toUpper() << ","
                << QString::number((receiveData.mid(126,2).toHex().toInt(nullptr,16))/16,16).toUpper() << ","
                << QString::number((receiveData.mid(127,2).toHex().toInt(nullptr,16))%4096,16).toUpper() << ","
                << QString::number((receiveData.mid(129,2).toHex().toInt(nullptr,16))/16,16).toUpper() << ","
                << receiveData.mid(79,2).toHex().toUpper() << ","

                << receiveData.mid(33,6).toHex().toUpper() << ","
                << receiveData.mid(39,6).toHex().toUpper() << ","
                << receiveData.mid(45,6).toHex().toUpper() << ","
                << receiveData.mid(51,6).toHex().toUpper() << ","
                << receiveData.mid(57,6).toHex().toUpper() << "\n";

//            out << QString::number((receiveData.mid(90,2).toHex().toInt(nullptr,16))/16,16).toUpper() << ","
//                << QString::number((receiveData.mid(91,2).toHex().toInt(nullptr,16))%4096,16).toUpper() << ","
//                << QString::number((receiveData.mid(93,2).toHex().toInt(nullptr,16))/16,16).toUpper() << ","
//                << QString::number((receiveData.mid(94,2).toHex().toInt(nullptr,16))%4096,16).toUpper() << ","
//                << receiveData.mid(70,2).toHex().toUpper() << ","

//                << QString::number((receiveData.mid(96,2).toHex().toInt(nullptr,16))/16,16).toUpper() << ","
//                << QString::number((receiveData.mid(97,2).toHex().toInt(nullptr,16))%4096,16).toUpper() << ","
//                << QString::number((receiveData.mid(99,2).toHex().toInt(nullptr,16))/16,16).toUpper() << ","
//                << QString::number((receiveData.mid(100,2).toHex().toInt(nullptr,16))%4096,16).toUpper() << ","
//                << receiveData.mid(72,2).toHex().toUpper() << ","

//                << QString::number((receiveData.mid(102,2).toHex().toInt(nullptr,16))/16,16).toUpper() << ","
//                << QString::number((receiveData.mid(103,2).toHex().toInt(nullptr,16))%4096,16).toUpper() << ","
//                << QString::number((receiveData.mid(105,2).toHex().toInt(nullptr,16))/16,16).toUpper() << ","
//                << QString::number((receiveData.mid(106,2).toHex().toInt(nullptr,16))%4096,16).toUpper() << ","
//                << receiveData.mid(74,2).toHex().toUpper() << ","

//                << QString::number((receiveData.mid(108,2).toHex().toInt(nullptr,16))/16,16).toUpper() << ","
//                << QString::number((receiveData.mid(109,2).toHex().toInt(nullptr,16))%4096,16).toUpper() << ","
//                << QString::number((receiveData.mid(111,2).toHex().toInt(nullptr,16))/16,16).toUpper() << ","
//                << QString::number((receiveData.mid(112,2).toHex().toInt(nullptr,16))%4096,16).toUpper() << ","
//                << receiveData.mid(76,2).toHex().toUpper() << ","

//                << QString::number((receiveData.mid(114,2).toHex().toInt(nullptr,16))/16,16).toUpper() << ","
//                << QString::number((receiveData.mid(115,2).toHex().toInt(nullptr,16))%4096,16).toUpper() << ","
//                << QString::number((receiveData.mid(117,2).toHex().toInt(nullptr,16))/16,16).toUpper() << ","
//                << QString::number((receiveData.mid(118,2).toHex().toInt(nullptr,16))%4096,16).toUpper() << ","
//                << QString::number((receiveData.mid(120,2).toHex().toInt(nullptr,16))/16,16).toUpper() << ","
//                << receiveData.mid(78,2).toHex().toUpper() << ","

//                << receiveData.mid(33,6).toHex().toUpper() << ","
//                << receiveData.mid(39,6).toHex().toUpper() << ","
//                << receiveData.mid(45,6).toHex().toUpper() << ","
//                << receiveData.mid(51,6).toHex().toUpper() << ","
//                << receiveData.mid(57,6).toHex().toUpper() << "\n";


        }
        data.close();
    }



    receiveData.clear();
    serialTimer.stop();

}
void MainWindow::handleReadyRead()
{
    serialTimer.start(200);
    receiveData = receiveData.append(serial->readAll());

    ui->textBrowserRec->append(receiveData.toHex().toUpper());


}
char MainWindow::convertHexChar(char ch)
{
    if ((ch >= '0') && (ch <= '9'))
        return ch - 0x30;
    else if ((ch >= 'A') && (ch <= 'F'))
        return ch - 'A' + 10;
    else if ((ch >= 'a') && (ch <= 'f'))
        return ch - 'a' + 10;
    else
        return ch -  ch;
}
QByteArray MainWindow::QStringToHex(QString str)
{
    QByteArray sendData;
    int hexdata,lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    sendData.resize(len / 2);//这里发送单个16进制数存在Bug
    char lstr,hstr;
    for (int i = 0; i < len; )
    {
        hstr = str[i].toLatin1();
        if (hstr == ' ')
        {
            ++i;
            continue;
        }
        ++i;
        if (i  >= len)
            break;
        lstr = str[i].toLatin1();
        hexdata = convertHexChar(hstr);
        lowhexdata = convertHexChar(lstr);
        if ((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata*16 + lowhexdata;
        ++i;
        sendData[hexdatalen] = (char)hexdata;
        ++hexdatalen;
    }
    sendData.resize(hexdatalen);
    return sendData;
}
void MainWindow::btnSendRST()
{
    QByteArray captureSendStr = MainWindow::QStringToHex(ui->textRST->toPlainText());
    if(serial->write(captureSendStr) == -1)
        QMessageBox::critical(this,"错误","???\n先打开串口!");
}
void MainWindow::btnSendTime()
{
    QByteArray captureSendStr = MainWindow::QStringToHex(ui->textTime->toPlainText());
    if(serial->write(captureSendStr) == -1)
        QMessageBox::critical(this,"错误","???\n先打开串口!");
}
void MainWindow::handleCheckbox()
{
    if(! ui->checkBoxTimer->isChecked())
        myTimer.stop();
}
void MainWindow::btnSendSend()
{
    if(ui->checkBoxTimer->isChecked())
    {
        myTimer.start(1000);
        QByteArray captureSendStr = MainWindow::QStringToHex(ui->textSend->toPlainText());
        if(serial->write(captureSendStr) == -1)
        {
            QMessageBox::critical(this,"错误","???\n先打开串口!");
            myTimer.stop();
            qDebug() << "错误";
        }
    }
    else
    {
        QByteArray captureSendStr = MainWindow::QStringToHex(ui->textSend->toPlainText());
        if(serial->write(captureSendStr) == -1)
            QMessageBox::critical(this,"错误","???\n先打开串口!");
    }
}
void MainWindow::btnSendTH1()
{
    QByteArray captureSendStr = MainWindow::QStringToHex(ui->textTH1->toPlainText());
    if(serial->write(captureSendStr) == -1)
        QMessageBox::critical(this,"错误","???\n先打开串口!");
}
void MainWindow::btnSendTH2()
{
    QByteArray captureSendStr = MainWindow::QStringToHex(ui->textTH2->toPlainText());
    if(serial->write(captureSendStr) == -1)
        QMessageBox::critical(this,"错误","???\n先打开串口!");
}
void MainWindow::btnSendTH3()
{
    QByteArray captureSendStr = MainWindow::QStringToHex(ui->textTH3->toPlainText());
    if(serial->write(captureSendStr) == -1)
        QMessageBox::critical(this,"错误","???\n先打开串口!");
}

void MainWindow::handleChartTimeOut()
{
    if(this->chartWindow.isHidden())
        chartTimer.stop();
    else
    {
        chartTimer.start();


//        qreal x =this->chartWindow.getChartView()->width() / axisX->tickCount();
//        qreal y = (axisX->max() - axisX->min()) / axisX->tickCount();


        qDebug() << dotNum << "," <<QString::number(receiveData.mid(70,2).toHex().toInt(nullptr,16),16);

    //    ui->textBrowser_28->setText(receiveData.mid(72,2).toHex().toUpper());
    //    ui->textBrowser_29->setText(receiveData.mid(74,2).toHex().toUpper());
    //    ui->textBrowser_30->setText(receiveData.mid(76,2).toHex().toUpper());
    //    ui->textBrowser_31->setText(receiveData.mid(78,2).toHex().toUpper());
//        dotNum += y;
        //qreal dwidth= chart.plotArea().width()/(axisX.tickCount()*2);
        series->append(dotNum,receiveData.mid(70,2).toHex().toInt(nullptr,16));
        dotNum++;
        if(dotNum>5000)
            theChart->scroll(1000,0);
        if(dotNum>50000)
        {
            dotNum = 0;
            chartTimer.stop();
        }
        //theChart->scroll(x, 0);

    }

}
void MainWindow::openChartWindow()
{
    this->chartWindow.show();
    chartTimer.start();



}

MainWindow::~MainWindow()
{
    delete ui;
}
