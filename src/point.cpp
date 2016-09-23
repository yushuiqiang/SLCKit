#include "point.h"
#include <QDebug>

Point::Point ()
    : m_x (NAN), m_y (NAN), m_z (NAN)
{}

Point::Point(qreal x, qreal y)
    : m_x (x), m_y (y), m_z (0.0)
{}

Point::Point (qreal x, qreal y, qreal z)
    : m_x (x), m_y (y), m_z (z)
{}

Point::Point (const Point &p)
    : m_x (p.m_x), m_y (p.m_y), m_z (p.m_z)
{}

void Point::setValue (qreal x, qreal y, qreal z)
{
    m_x = x;
    m_y = y;
    m_z = z;
}

void Point::setValue(const Point &p)
{
    m_x = p.m_x;
    m_y = p.m_y;
    m_z = p.m_z;
}

void Point::setX (qreal x)
{
    m_x = x;
}

void Point::setY (qreal y)
{
    m_y = y;
}

void Point::setZ (qreal z)
{
    m_z = z;
}

qreal Point::length () const
{
    qreal length (INFINITY);
    if (isValid ())
        length = sqrt(m_x * m_x + m_y * m_y + m_z * m_z);
    return length;
}

qreal Point::distance(const Point &other) const
{
    return (*this - other).length ();
}

qreal Point::length2D() const
{
    qreal length (INFINITY);
    if (isValid ())
        length = sqrt(m_x * m_x + m_y * m_y);
    return length;
}

qreal Point::distance2D(const Point &other) const
{
    return (*this - other).length2D ();
}

qreal Point::x () const
{
    return m_x;
}

qreal Point::y () const
{
    return m_y;
}

qreal Point::z () const
{
    return m_z;
}

Point &Point::operator += (const Point &p)
{
    m_x += p.m_x;
    m_y += p.m_y;
    m_z += p.m_z;
    return *this;
}

Point &Point::operator -= (const Point &p)
{
    m_x -= p.m_x;
    m_y -= p.m_y;
    m_z -= p.m_z;
    return *this;
}

Point &Point::operator *= (const qreal scale)
{
    m_x *= scale;
    m_y *= scale;
    m_z *= scale;
    return *this;
}

Point &Point::operator *= (const Point &p)
{
    m_x *= p.m_x;
    m_y *= p.m_y;
    m_z *= p.m_z;
    return *this;
}

Point &Point::operator /= (const qreal scale)
{
    if (!fuzzyIsNull (scale))
    {
        m_x /= scale;
        m_y /= scale;
        m_z /= scale;
    }
    return *this;
}

Point &Point::operator /= (const Point &p)
{
    if (!fuzzyIsNull (p.m_x) && !fuzzyIsNull (p.m_y) && !fuzzyIsNull (p.m_z))
    {
        m_x /= p.m_x;
        m_y /= p.m_y;
        m_z /= p.m_z;
    }
    return *this;
}

bool Point::isValid () const
{
    bool valid = std::isfinite (m_x) && std::isfinite (m_y) && std::isfinite (m_z);
    return valid;
}

bool Point::isZero () const
{
    bool isZero = (fuzzyIsNull (m_x) && fuzzyIsNull (m_y) && fuzzyIsNull (m_z));
    return isZero;
}

const QString Point::string () const
{
    QString line (QString ("(%1,%2,%3)").arg ((double) m_x).arg ((double) m_y).arg ((double) m_z));
    return line;
}

const Point Point::zero()
{
    return Point (0.0, 0.0, 0.0);
}

const Point Point::nan()
{
    Point invalid;
    return invalid;
}

bool Point::operator== (const Point &other) const
{
    return (fuzzyIsEqual (m_x, other.m_x) && fuzzyIsEqual (m_y, other.m_y) && fuzzyIsEqual (m_z, other.m_z));
}

bool Point::operator!= (const Point &other) const
{
    return ! operator == (other);
}

const Point operator+ (const Point &p, const Point &q)
{
    Point t (p);
    t.operator += (q);
    return t;
}

const Point operator- (const Point &p, const Point &q)
{
    Point t (p);
    t.operator -= (q);
    return t;
}

const Point operator* (const Point &p, const qreal scale)
{
    Point t (p);
    t.operator *=(scale);
    return t;
}

const Point operator* (const Point &p, const Point &q)
{
    Point t (p);
    t.operator *=(q);
    return t;
}

const Point operator/ (const Point &p, const qreal scale)
{
    Point t (p);
    t.operator /= (scale);
    return t;
}

const Point operator/ (const Point &p, const Point &q)
{
    Point t (p);
    t.operator /= (q);
    return t;
}

QDebug operator << (QDebug dbg, const Point &point)
{
    dbg.nospace () << '(' << point.x () << ',' << point.y () << ',' << point.z () << ')';
    return dbg.space ();
}

QDataStream &operator << (QDataStream &stream, const Point &p)
{
    stream << p.x();
    stream << p.y();
    stream << p.z();
    return stream;
}

QDataStream &operator >> (QDataStream &stream, Point &p)
{
    qreal x,y,z;
    stream >> x;
    stream >> y;
    stream >> z;
    p.setValue(x,y,z);
    return stream;
}

