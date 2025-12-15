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
        const QColor backGround(239, 239, 239);
        const QColor light = backGround.lighter(150);
        const QColor mid(backGround.darker(130));
        const QColor midLight = mid.lighter(110);
        const QColor base = Qt::white;
        const QColor disabledBase(backGround);
        const QColor dark = backGround.darker(150);
        const QColor darkDisabled = QColor(209, 209, 209).darker(110);
        const QColor text = Qt::black;
        const QColor hightlightedText = Qt::white;
        const QColor disabledText = QColor(190, 190, 190);
        const QColor button = backGround;
        const QColor shadow = dark.darker(135);
        const QColor disabledShadow = shadow.lighter(150);
        QPalette pal(Qt::black, backGround, light, dark, mid, text, base);
        pal.setBrush(QPalette::Midlight, midLight);
        pal.setBrush(QPalette::Button, button);
        pal.setBrush(QPalette::Shadow, shadow);
        pal.setBrush(QPalette::HighlightedText, hightlightedText);
        pal.setBrush(QPalette::Disabled, QPalette::Text, disabledText);
        pal.setBrush(QPalette::Disabled, QPalette::WindowText, disabledText);
        pal.setBrush(QPalette::Disabled, QPalette::ButtonText, disabledText);
        pal.setBrush(QPalette::Disabled, QPalette::Base, disabledBase);
        pal.setBrush(QPalette::Disabled, QPalette::Dark, darkDisabled);
        pal.setBrush(QPalette::Disabled, QPalette::Shadow, disabledShadow);
        pal.setBrush(QPalette::Active, QPalette::Highlight, QColor(48, 140, 198));
        pal.setBrush(QPalette::Inactive, QPalette::Highlight, QColor(48, 140, 198));
        pal.setBrush(QPalette::Disabled, QPalette::Highlight, QColor(145, 145, 145));
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
    a.setPalette(PaletteGuard(&a).lightPalette());
    a.installEventFilter(new PaletteGuard(&a));
#endif

    MainWindow w;
    w.show();
    return a.exec();
}
