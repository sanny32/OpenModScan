#include <QApplication>
#include "mainwindow.h"

///
/// \brief The PaletteGuard class
///
class PaletteGuard : public QObject {
public:
    PaletteGuard(QObject* parent) : QObject(parent) { }
    bool eventFilter(QObject *obj, QEvent *ev) override {
        if (ev->type() == QEvent::ApplicationPaletteChange) {
            QTimer::singleShot(0, [](){
                QApplication::setPalette(QApplication::style()->standardPalette());
            });
        }
        return QObject::eventFilter(obj, ev);
    }
};

///
/// \brief main
/// \param argc
/// \param argv
/// \return
///
int main(int argc, char *argv[])
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    QApplication::setDesktopSettingsAware(false);
#endif

    QApplication a(argc, argv);
    a.setApplicationName(APP_NAME);
    a.setApplicationVersion(APP_VERSION);

#ifdef Q_OS_WIN
    a.setStyle("windowsvista");
#else
    a.setStyle("Fusion");
    qApp->installEventFilter(new PaletteGuard(qApp));
#endif

    MainWindow w;
    w.show();
    return a.exec();
}
