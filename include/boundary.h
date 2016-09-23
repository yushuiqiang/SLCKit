#ifndef BOUNDARY_H
#define BOUNDARY_H

#include "point.h"

class SLCKIT_EXPORT Boundary
{
public:
    Boundary ();
    Boundary (const Point &initial);

    qreal minX () const;
    qreal maxX () const;
    qreal minY () const;
    qreal maxY () const;
    qreal minZ () const;
    qreal maxZ () const;

    void setMinX (const qreal value);
    void setMaxX (const qreal value);
    void setMinY (const qreal value);
    void setMaxY (const qreal value);
    void setMinZ (const qreal value);
    void setMaxZ (const qreal value);

    void refer (const Point &point);
    void refer (const Boundary &boundary);

    const Point center () const;
    const Point dimension () const;

    bool isValid () const;

    qreal volumn () const;
    qreal area () const;

    bool operator < (const Boundary &other) const;

    QString string () const;

private:
    Point m_min;
    Point m_max;
};

SLCKIT_EXPORT QDebug operator << (QDebug dbg, const Boundary &boundary);

SLCKIT_EXPORT QDataStream &operator << (QDataStream &stream, const Boundary &boundary);
SLCKIT_EXPORT QDataStream &operator >> (QDataStream &stream, Boundary &boundary);

Q_DECLARE_METATYPE (Boundary)

#endif // BOUNDARY_H
