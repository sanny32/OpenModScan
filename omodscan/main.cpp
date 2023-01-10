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
    a.setOrganizationName(APP_NAME);
    a.setOrganizationDomain(APP_DOMAIN);
    a.setApplicationName(APP_VERSION);
    a.setApplicationVersion(APP_VERSION);

    MainWindow w;
    w.show();
    return a.exec();
}
