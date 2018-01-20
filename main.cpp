#include "plotwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    PlotWidget view;
    view.show();

    return app.exec();
}
