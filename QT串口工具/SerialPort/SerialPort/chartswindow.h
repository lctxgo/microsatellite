#ifndef CHARTSWINDOW_H
#define CHARTSWINDOW_H

#include <QMainWindow>
#include <QtCharts>
QT_CHARTS_USE_NAMESPACE

namespace Ui {
class chartsWindow;
}

class chartsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit chartsWindow(QWidget *parent = nullptr);
    ~chartsWindow();
    QChartView *getChartView();
private:
    Ui::chartsWindow *ui;


};

#endif // CHARTSWINDOW_H
