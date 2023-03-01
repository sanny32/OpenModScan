#ifndef QRANGE_H
#define QRANGE_H

#include <limits>
#include <QDataStream>

///
/// \brief The QRange class
///
template<class T>
class QRange
{

    friend QDataStream& operator <<(QDataStream& out, const QRange<double>& range);
    friend QDataStream& operator >>(QDataStream& in, QRange<double>& range);

public:
    ///
    /// \brief QRange
    ///
    explicit QRange()
    {
        _from = std::numeric_limits<T>::min();
        _to = std::numeric_limits<T>::max();
    }

    ///
    /// \brief QRange
    /// \param from
    /// \param to
    ///
    QRange(T from, T to)
    {
        _from = from;
        _to = to;
    }

    ///
    /// \brief from
    /// \return
    ///
    T from() const { return _from;}

    ///
    /// \brief to
    /// \return
    ///
    T to() const { return _to; };

    ///
    /// \brief contains
    /// \param num
    /// \return
    ///
    bool contains(T num) const
    {
        return num >= _from && num <= _to;
    }

private:
    T _from;
    T _to;
};

///
/// \brief operator <<
/// \param out
/// \param range
/// \return
///
inline QDataStream& operator <<(QDataStream& out, const QRange<double>& range)
{
    out << range.from();
    out << range.to();
    return out;
}

///
/// \brief operator >>
/// \param in
/// \param range
/// \return
///
inline QDataStream& operator >>(QDataStream& in, QRange<double>& range)
{
    in >> range._from;
    in >> range._to;
    return in;
}

#endif // QRANGE_H
