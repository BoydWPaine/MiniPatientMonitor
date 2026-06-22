#include "ui/main_window.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    mpm::host::MainWindow window;
    window.show();
    return app.exec();
}