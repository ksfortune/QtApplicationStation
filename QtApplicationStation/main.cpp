#include <QApplication>
#include <QWidget>
#include "Windows.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    StartWindow window;
    window.show();
    return app.exec();
}
