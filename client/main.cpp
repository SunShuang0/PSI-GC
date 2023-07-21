#include "widget.h"
#include "choose.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);



//    Choose c;
//    c.show();

    Widget w;
    w.show();

    return a.exec();
}
