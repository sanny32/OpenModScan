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
            QTimer::singleShot(0, [this](){
                QApplication::setPalette(lightPalette());
            });
        }
        return QObject::eventFilter(obj, ev);
    }

    QPalette lightPalette() const {
        QPalette pal;
        pal.setColor(QPalette::Window, QColor(239,239,239));
        pal.setColor(QPalette::WindowText, Qt::black);
        pal.setColor(QPalette::Base, Qt::white);
        pal.setColor(QPalette::AlternateBase, QColor(246,246,246));
        pal.setColor(QPalette::Text, Qt::black);
        pal.setColor(QPalette::Button, QColor(239,239,239));
        pal.setColor(QPalette::ButtonText, Qt::black);
        pal.setColor(QPalette::Highlight, QColor(48,140,198));
        pal.setColor(QPalette::HighlightedText, Qt::white);
        return pal;
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
    QApplication a(argc, argv);
    a.setApplicationName(APP_NAME);
    a.setApplicationVersion(APP_VERSION);

#ifdef Q_OS_WIN
    a.setStyle("windowsvista");
#else
    a.setStyle("Fusion");
    a.installEventFilter(new PaletteGuard(qApp));
#endif

    MainWindow w;
    w.show();
    return a.exec();
}
