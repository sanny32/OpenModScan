#ifndef QRANGE_H
#define QRANGE_H

#include <limits>

///
/// \brief The QRange class
///
template<class T>
class QRange
{
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

#endif // QRANGE_H
