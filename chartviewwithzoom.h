#ifndef CHARTVIEWWITHZOOM_H
#define CHARTVIEWWITHZOOM_H
#include <QtCharts/qchartview.h>
#include <QtWidgets/QRubberBand>

using namespace QtCharts;

class ChartViewWithZoom : public QChartView
{
public:
    ChartViewWithZoom(QWidget *parent = nullptr);
    ChartViewWithZoom(QChart *chart, QWidget *parent = nullptr);
protected:
    bool viewPortEvent(QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent * event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
private:
    bool m_isTouching;
};


#endif // CHARTVIEWWITHZOOM_H
