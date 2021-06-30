#include "chartswindow.h"
#include "ui_chartswindow.h"
#include <QDateTime>
#include <QChartView>
chartsWindow::chartsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::chartsWindow)
{
    ui->setupUi(this);



    //创建创建X轴和Y轴
/*    QValueAxis *axisX = new QValueAxis;
    axisX->setRange(0,150); */   //改为setRange(0,1);则表示坐标为动态计算大小的
//    axisX->setLabelFormat("%dS");
//    QValueAxis *axisY = new QValueAxis;
//    axisY->setRange(0,250);    //改为setRange(0,1);则表示坐标为动态计算大小的
//    axisY->setTitleText("value值");



}
//void chartsWindow::funk()
//{
//    this->show();
//    QChart *theChart = new QChart();
//    theChart->setTheme(QChart::ChartThemeLight);//设置light主题
//    theChart->setTitle("探测器照度");
////    QValueAxis *axisY = new QValueAxis;
////    axisY->setRange(0,65536);

//////    QDateTimeAxis *timeAxis = new QDateTimeAxis;
//////    timeAxis->setTickCount(10);//这里为什么用10
//////    timeAxis->setFormat("hh:mm:ss");
//////    timeAxis->setTitleText("时间");
////    QValueAxis *axisX = new QValueAxis;
////    axisX->setRange(0,65536);
////    theChart->setAxisX(axisX,window.series);
////    theChart->setAxisY(axisY,window.series);
//    QSplineSeries *series1 = new QSplineSeries();
//    theChart->createDefaultAxes();
//    theChart->addSeries(series1);
//    ui->Chartview->setChart(theChart);

//    QSplineSeries *series2 = MainWindow::getSeries();
//    this->window.isHidden();
//}
QChartView *chartsWindow::getChartView()
{
    return ui->Chartview;
}

chartsWindow::~chartsWindow()
{
    delete ui;
}
