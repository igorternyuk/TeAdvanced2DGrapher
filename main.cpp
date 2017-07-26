#include "widget.h"
#include <QApplication>
#include <QDesktopWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    a.setStyle("fusion");
    QRect myScreenGeometry = QApplication::desktop()->geometry() ;
        int x = (myScreenGeometry.width() - w.width()) / 2;
        int y = (myScreenGeometry.height() - w.height()) / 2;
        w.move(x, y);
    w.grabGesture(Qt::PanGesture);
    w.grabGesture(Qt::PinchGesture);
    w.show();
    return a.exec();
}
