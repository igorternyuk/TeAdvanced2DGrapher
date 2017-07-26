#include "chartviewwithzoom.h"
#ifdef DEBUG
#include <QDebug>
#endif

ChartViewWithZoom::ChartViewWithZoom(QWidget *parent) :
    QChartView(parent), m_isTouching(false)
{
    setRubberBand(QChartView::RectangleRubberBand);
}
ChartViewWithZoom::ChartViewWithZoom(QChart *chart, QWidget *parent) :
    QChartView(chart, parent),
    m_isTouching(false)
{
    setRubberBand(QChartView::RectangleRubberBand);
}

bool ChartViewWithZoom::viewPortEvent(QEvent *event)
{
    if(event->type() == QEvent::TouchBegin)
    {
        m_isTouching = true;
        this->chart()->setAnimationOptions(QChart::NoAnimation);
    }
    return QChartView::viewportEvent(event);
}

void ChartViewWithZoom::mousePressEvent(QMouseEvent *event)
{
   if(m_isTouching) return;
   // Делаем чтобы при нажатии колесика восстанавливался первоначальный масштаб
   if(event->button() == Qt::MiddleButton)
   {
       chart()->zoomReset();
       return;
   }
   QChartView::mousePressEvent(event);
}

void ChartViewWithZoom::mouseMoveEvent(QMouseEvent *event)
{
   if(m_isTouching) return;
   QChartView::mouseMoveEvent(event);
}

void ChartViewWithZoom::mouseReleaseEvent(QMouseEvent *event)
{
  if(m_isTouching) return;
  QChartView::mouseReleaseEvent(event);
}

void ChartViewWithZoom::wheelEvent(QWheelEvent *event)
{
     double factor = event->delta() / 120;
     if(factor > 0)
         chart()->zoom(1.1);
     else
         chart()->zoom(0.9);
}

void ChartViewWithZoom::keyPressEvent(QKeyEvent *event)
{
   switch(event->key())
   {
     case Qt::Key_Plus:
          chart()->zoom(1.1);
          break;
     case Qt::Key_Minus:
          chart()->zoom(0.9);
          break;
     case Qt::Key_F9:
          chart()->zoomReset();
          break;
     case Qt::Key_Left:
          chart()->scroll(10, 0);
          break;
     case Qt::Key_Right:
          chart()->scroll(-10, 0);
          break;
     case Qt::Key_Up:
          chart()->scroll(0, -10);
          break;
     case Qt::Key_Down:
          chart()->scroll(0, 10);
          break;
     default:
          QGraphicsView::keyPressEvent(event);
          break;
   }
}
