#ifndef QMODBUSADU_H
#define QMODBUSADU_H

#include <QModbusPdu>
#include <QByteArray>

///
/// \brief The QModbusAdu class
///
class QModbusAdu
{
public:
    ///
    /// \brief QModbusAdu
    /// \param type
    /// \param data
    ///
    explicit QModbusAdu() = default;
    virtual ~QModbusAdu() = default;

    ///
    /// \brief isValid
    /// \return
    ///
    virtual bool isValid() const = 0;

    ///
    /// \brief rawData
    /// \return
    ///
    QByteArray rawData() const { return _data; }

    ///
    /// \brief setRawData
    /// \param data
    ///
    virtual void setRawData(const QByteArray& data)  = 0;

    ///
    /// \brief serverAddress
    /// \return
    ///
    virtual quint8 serverAddress() const  = 0;

    ///
    /// \brief functionCode
    /// \return
    ///
    QModbusPdu::FunctionCode functionCode() const {
        return _pdu.functionCode();
    }

    ///
    /// \brief exceptionCode
    /// \return
    ///
    QModbusPdu::ExceptionCode exceptionCode() const {
        return _pdu.exceptionCode();
    }

    ///
    /// \brief isException
    /// \return
    ///
    bool isException() const {
        return _pdu.isException();
    }

    ///
    /// \brief pdu
    /// \return
    ///
    const QModbusPdu& pdu() const {
        return _pdu;
    }

protected:
    QByteArray _data;
    QModbusPdu _pdu;
};

#endif // QMODBUSADU_H
