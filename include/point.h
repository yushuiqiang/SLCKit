#ifndef POINT_H
#define POINT_H

#include "math.hpp"
#include <QDebug>
#include <QDataStream>

class SLCKIT_EXPORT Point
{
public:
    Point ();
    Point (qreal x, qreal y);
    Point (qreal x, qreal y, qreal z);
    Point (const Point &p);

    void setValue (qreal x, qreal y, qreal z);
    void setValue (const Point &p);
    void setX (qreal x);
    void setY (qreal y);
    void setZ (qreal z);

    qreal length () const;
    qreal distance (const Point &other) const;

    qreal length2D () const;
    qreal distance2D (const Point &other) const;

    qreal x() const;
    qreal y() const;
    qreal z() const;

    Point &operator+= (const Point &p);
    Point &operator-= (const Point &p);
    Point &operator*= (const qreal scale);
    Point &operator*= (const Point &p);
    Point &operator/= (const qreal scale);
    Point &operator/= (const Point &p);

    bool isValid () const;
    bool isZero () const;

    const QString string () const;

    bool operator== (const Point &other) const;
    bool operator!= (const Point &other) const;

    static const Point zero ();
    static const Point nan ();

private:
    qreal m_x, m_y, m_z;
};

SLCKIT_EXPORT const Point operator+ (const Point &p, const Point &q);
SLCKIT_EXPORT const Point operator- (const Point &p, const Point &q);
SLCKIT_EXPORT const Point operator* (const Point &p, const qreal scale);
SLCKIT_EXPORT const Point operator* (const Point &p, const Point &q);
SLCKIT_EXPORT const Point operator/ (const Point &p, const qreal scale);
SLCKIT_EXPORT const Point operator/ (const Point &p, const Point &q);

SLCKIT_EXPORT QDebug operator << (QDebug dbg, const Point &point);
SLCKIT_EXPORT QDataStream &operator << (QDataStream &stream, const Point &p);
SLCKIT_EXPORT QDataStream &operator >> (QDataStream &stream, Point &p);

Q_DECLARE_METATYPE (Point)

#endif // POINT_H
