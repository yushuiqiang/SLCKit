#ifndef LAYER_H
#define LAYER_H

#include "polygon.h"

class SLCKIT_EXPORT Layer : public QVector<Polygon>
{
public:
    class InfillSpec
    {
    public:
        enum InfillType
        {
            Line,
            Zigzag,
            ZigzagContinuous,
            Concentric,
        };

        InfillType type = Line;
        qreal interval = 1;
        qreal angle = 0;
        qreal shrinkWidth = 0;
        qint8 extraContourCount = 0;
        qreal extraContourWidth = 0;

        bool operator == (const InfillSpec &other) const;
        bool operator != (const InfillSpec &other) const;
    };

    enum SortPattern
    {
        SupportInfillContour,
        SupportContourInfill,
    };

    void setThickness (const qreal thickness);
    qreal thickness () const;

    void setHeight (const qreal height);
    qreal height () const;

    const Layer offsetted (const qreal delta) const;
    void offset (const qreal delta);

    const Layer translated (const Point &offset) const;
    void translate (const Point &offset);

    const Boundary boundary () const;
    const Point center () const;
    const Point dimension () const;

    const QString string () const;

    bool operator < (const Layer &other) const;

    void sort (SortPattern pattern);
    const Layer sorted (SortPattern pattern) const;

    const Point optimize (const Point &reference = Point ());
    const Layer optimized (const Point &reference = Point ()) const;

    const Point optimize (SortPattern pattern, const Point &reference = Point ());
    const Layer optimized (SortPattern pattern, const Point &reference = Point ()) const;

    void filter (Polygon::PolygonType type);
    const Layer filtered (Polygon::PolygonType type) const;

    qreal area () const;

private:
    qreal m_thickness = 0.0;
    qreal m_height = 0.0;
};

SLCKIT_EXPORT QDebug operator << (QDebug dbg, const Layer &layer);

SLCKIT_EXPORT QDataStream &operator << (QDataStream &stream, const Layer &layer);
SLCKIT_EXPORT QDataStream &operator >> (QDataStream &stream, Layer &layer);

Q_DECLARE_METATYPE (Layer)

#endif // LAYER_H
