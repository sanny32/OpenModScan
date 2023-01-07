#ifndef TEXTCAPTUREHANDLER_H
#define TEXTCAPTUREHANDLER_H

#include <QObject>
#include <QFile>
#include <QModbusReply>

class FormModSca;

///
/// \brief The TextCaptureHandler class
///
class TextCaptureHandler : public QObject
{
    Q_OBJECT
public:
    explicit TextCaptureHandler(FormModSca* parent);
    ~TextCaptureHandler() override;

    void startCapture(const QString& file);
    void stopCapture();

    void update(QModbusReply* reply);

private:
    QFile _file;
};

#endif // TEXTCAPTUREHANDLER_H
