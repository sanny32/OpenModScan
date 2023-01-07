#include "formmodsca.h"
#include "textcapturehandler.h"

///
/// \brief TextCaptureHandler::TextCaptureHandler
/// \param parent
///
TextCaptureHandler::TextCaptureHandler(FormModSca* parent)
    : QObject{parent}
{
    Q_ASSERT(parent != nullptr);
}

///
/// \brief TextCaptureHandler::~TextCaptureHandler
///
TextCaptureHandler::~TextCaptureHandler()
{
    stopCapture();
}

///
/// \brief TextCaptureHandler::startCapture
/// \param file
///
void TextCaptureHandler::startCapture(const QString& file)
{

}

///
/// \brief TextCaptureHandler::stopCapture
///
void TextCaptureHandler::stopCapture()
{

}

///
/// \brief TextCaptureHandler::update
/// \param reply
///
void TextCaptureHandler::update(QModbusReply* reply)
{

}
