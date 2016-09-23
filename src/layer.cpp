#include "layer.h"

void Layer::setThickness (const qreal thickness)
{
    m_thickness = thickness;
}

qreal Layer::thickness () const
{
    return m_thickness;
}

void Layer::setHeight(const qreal height)
{
    m_height = height;
}

qreal Layer::height() const
{
    return m_height;
}

const Layer Layer::translated(const Point &offset) const
{
    Layer other (*this);
    other.translate (offset);
    return other;
}

void Layer::translate(const Point &offset)
{
    for (Polygon &polygon : *this)
    {
        polygon.translate (offset);
    }
}

const Boundary Layer::boundary () const
{
    Boundary boundary;
    for (const Polygon &polygon : *this)
    {
        boundary.refer (polygon.boundary ());
    }
    return boundary;
}

const Point Layer::center() const
{
    return boundary ().center ();
}

const Point Layer::dimension() const
{
    return boundary ().dimension ();
}

const QString Layer::string () const
{
    QString line;
    line.append('{');
    for (const Polygon &polygon : *this)
    {
        line.append (polygon.string ());
    }
    line.append ('}');
    return line;
}

bool Layer::operator < (const Layer &other) const
{
    return (m_height < other.m_height);
}

int priority (Layer::SortPattern pattern, const Polygon::PolygonType type)
{
    int value (0);
    switch (pattern)
    {
    case Layer::SupportInfillContour:
        {
            switch (type)
            {
            case Polygon::Support:
                value = 0;
                break;
            case Polygon::Infill:
                value = 1;
                break;
            case Polygon::Extra:
                value = 2;
                break;
            case Polygon::Contour:
                value = 3;
                break;
            }
        }
        break;

    case Layer::SupportContourInfill:
        {
            switch (type)
            {
            case Polygon::Support:
                value = 0;
                break;
            case Polygon::Extra:
                value = 1;
                break;
            case Polygon::Contour:
                value = 2;
                break;
            case Polygon::Infill:
                value = 3;
                break;
            }
        }
        break;
    }
    return value;
};

void Layer::sort(Layer::SortPattern pattern)
{
    auto functor = [&] (const Polygon &a, const Polygon &b)
    {
        int aValue = priority (pattern, a.type ());
        int bValue = priority (pattern, b.type ());
        return (aValue < bValue);
    };

    std::stable_sort (begin (), end (), functor);
}

const Layer Layer::sorted(Layer::SortPattern pattern) const
{
    Layer other (*this);
    other.sort (pattern);
    return other;
}

const Point Layer::optimize(const Point &reference)
{
    int N = count ();
    Point last = reference;

    // 0,1,2 elements
    if (N < 1)
    {
        return last;
    }

    int i = 0;
    for (; i < N; ++i)
    {
        Polygon &polygon = (*this) [i];
        if (!polygon.isEmpty ())
        {
            if (polygon.first ().distance2D (last) > polygon.last ().distance2D (last))
            {
                polygon.reverse ();
            }

            last = polygon.last ();
            break;
        }
    }

    for (; i < N - 1; ++i)
    {
        const Polygon &polygon = at (i);

        // if empty
        if (polygon.isEmpty ())
        {
            continue;
        }

        QVector<qreal> distance1, distance2;

        int next = i + 1;

        for (int j = next; j < N; ++j)
        {
            const Polygon &p (at (j));
            if (!p.isEmpty ())
            {
                distance1.append (p.first ().distance2D (last));
                distance2.append (p.last ().distance2D (last));
            }
            else
            {
                distance1.append (INFINITY);
                distance2.append (INFINITY);
            }
        }

        int index (next);

        int index1 = std::min_element (distance1.cbegin (), distance1.cend ()) - distance1.cbegin ();
        int index2 = std::min_element (distance2.cbegin (), distance2.cend ()) - distance2.cbegin ();

        qreal d1 = distance1.at (index1);
        qreal d2 = distance2.at (index2);

        bool flip (false);
        if (d2 < d1)
        {
            flip = true;
            index += index2;
        }
        else
        {
            index += index1;
        }

        Polygon &current ((*this)[next]);
        Polygon &target  ((*this)[index]);

        // last = target.constLast ();

        if (flip)
        {
            target.reverse ();
        }

        last = target.last ();

        std::swap (current, target);
    }
    return last;
}

const Layer Layer::optimized(const Point &reference) const
{
    Layer other (*this);
    other.optimize (reference);
    return other;
}

const Point Layer::optimize(Layer::SortPattern pattern, const Point &reference)
{
    sort (pattern);

    Point last = reference;

    Layer flatterned [Polygon::PolygonTypeCount];
    for (const Polygon &polygon : *this)
    {
        int p (priority (pattern, polygon.type ()));
        flatterned [p].append (polygon);
    }

    clear ();

    for (int i = 0; i < Polygon::PolygonTypeCount; ++i)
    {
        Layer &layer = flatterned[i];
        last = layer.optimize (last);
        append (layer);
    }
    return last;
}

const Layer Layer::optimized(Layer::SortPattern pattern, const Point &reference) const
{
    Layer other (*this);
    other.optimize (pattern, reference);
    return other;
}

void Layer::filter(Polygon::PolygonType type)
{
    for (int i = count (); i >= 0; --i)
    {
        const Polygon &polygon = at (i);
        if (type != polygon.type ())
        {
            removeAt (i);
        }
    }
}

const Layer Layer::filtered(Polygon::PolygonType type) const
{
    Layer other (*this);
    other.filter (type);
    return other;
}

qreal Layer::area() const
{
    qreal area = std::accumulate (begin (),
                                  end (),
                                  qreal (0.0),
                                  [] (qreal sum, const Polygon &p) -> qreal {return sum + p.area ();});
    return area;
}

QDebug operator << (QDebug dbg, const Layer &layer)
{
    dbg.nospace () << '{';
    dbg << '<' << layer.thickness () << '@' << layer.height () << '>';
    for (const Polygon &polygon : layer)
    {
        dbg << polygon;
    }
    dbg << '}';
    return dbg.space ();
}

QDataStream &operator >> (QDataStream &stream, Layer &layer)
{
    qreal thickness, height;
    stream >> thickness;
    stream >> height;
    stream >> *((QVector<Polygon>*)&layer);
    layer.setThickness(thickness);
    layer.setHeight (height);
    return stream;
}

QDataStream &operator << (QDataStream &stream, const Layer &layer)
{
    stream << layer.thickness ();
    stream << layer.height ();
    stream << *((QVector<Polygon>*)&layer);
    return stream;
}
