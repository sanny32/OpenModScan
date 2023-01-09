#include <QApplication>
#include "mainwindow.h"

///
/// \brief main
/// \param argc
/// \param argv
/// \return
///
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationVersion(APP_VERSION);

    MainWindow w;
    w.show();
    return a.exec();
}
