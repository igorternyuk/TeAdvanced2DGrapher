#ifndef CUSTOMCHART_H
#define CUSTOMCHART_H
#include <QtCharts/QChart>

using namespace QtCharts;

class QGestureEvent;
class CustomChart : public QChart
{
public:
      explicit CustomChart(QGraphicsItem *parent = nullptr, Qt::WindowFlags wFlags = 0);
protected:
      bool sceneEvent(QEvent *event);

  private:
      bool gestureEvent(QGestureEvent *event);
};

#endif // CUSTOMCHART_H
