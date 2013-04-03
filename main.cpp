#include <QApplication>
#include "Musec.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    Musec musec;
    musec.show();
    return app.exec();
}
