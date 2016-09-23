#ifndef POLYGON_H
#define POLYGON_H

#include <QVector>
#include "point.h"
#include "boundary.h"

class SLCKIT_EXPORT Polygon : public QVector<Point>
{
public:
	enum PolygonType
	{
        Contour         = 0,
        Infill          = 1,
        Support         = 2,
        Extra           = 3,

        PolygonTypeCount = Extra + 1,
    };

    void setType (PolygonType type);
    PolygonType type () const;

    void simplify ();
    Polygon simplified () const;

    void reverse ();
    Polygon reversed () const;

    void close ();
    Polygon closed () const;
    bool isClosed () const;

    const Polygon translated (const Point &offset) const;
    void translate (const Point &offset);

    const Boundary boundary () const;

    qreal area () const;
    const Point centroid () const;
    const Point center () const;
    const Point dimension () const;

    const QString string () const;

    bool operator < (const Polygon &other) const;

private:
    PolygonType m_type = Contour;
};

SLCKIT_EXPORT QDataStream &operator << (QDataStream &stream, const Polygon &polygon);
SLCKIT_EXPORT QDataStream &operator >> (QDataStream &stream, Polygon &polygon);

#endif // POLYGON_H
