#include "polygon.h"
#include <algorithm>

void Polygon::setType (Polygon::PolygonType type)
{
    m_type = type;
}

Polygon::PolygonType Polygon::type () const
{
    return m_type;
}

void Polygon::simplify()
{
    for (int i = count () - 2; i > 0; --i)
    {
        if (count () < 3)
            break;

        const Point prevDelta ( (*this)[i] - (*this)[i-1]);
        const Point nextDelta ( (*this)[i+1] - (*this)[i]);

        if (nextDelta.isZero ())
        {
            removeAt (i);
            continue;
        }

        qreal prevDeltaLength (prevDelta.length ());
        qreal prevDeltaSlopeX (prevDelta.x () / prevDeltaLength);
        qreal prevDeltaSlopeY (prevDelta.y () / prevDeltaLength);
        qreal prevDeltaSlopeZ (prevDelta.z () / prevDeltaLength);

        qreal nextDeltaLength (nextDelta.length ());
        qreal nextDeltaSlopeX (nextDelta.x () / nextDeltaLength);
        qreal nextDeltaSlopeY (nextDelta.y () / nextDeltaLength);
        qreal nextDeltaSlopeZ (nextDelta.z () / nextDeltaLength);

        if (fuzzyIsEqual (prevDeltaSlopeX, nextDeltaSlopeX) &&
            fuzzyIsEqual (prevDeltaSlopeY, nextDeltaSlopeY) &&
            fuzzyIsEqual (prevDeltaSlopeZ, nextDeltaSlopeZ))
        {
            removeAt (i);
        }
    }
}

Polygon Polygon::simplified() const
{
    Polygon other (*this);
    other.simplify ();
    return other;
}

void Polygon::reverse ()
{
    std::reverse(this->begin(), this->end());
}

Polygon Polygon::reversed () const
{
    Polygon other (*this);
    other.reverse();
    return other;
}

void Polygon::close ()
{
    if (!isClosed())
    {
        this->append (this->front());
    }
}

Polygon Polygon::closed () const
{
    Polygon other (*this);
    other.close();
    return other;
}

bool Polygon::isClosed () const
{
    // 空路径亦是闭合的
    bool isClosed (true);
    if (!isEmpty ())
        isClosed = (this->front() == this->back());
    return isClosed;
}

const Polygon Polygon::translated(const Point &offset) const
{
    Polygon other (*this);
    other.translate (offset);
    return other;
}

void Polygon::translate(const Point &offset)
{
    if (! offset.isValid ())
    {
        return;
    }

    for (Point &point : *this)
    {
        point += offset;
    }
}

const Boundary Polygon::boundary() const
{
    Boundary boundary;
    for (const Point &point : *this)
    {
        boundary.refer (point);
    }
    return boundary;
}

// Code below adopted from
// http://alienryderflex.com/polygon_area/
// and later based on
// http://stackoverflow.com/questions/2792443/finding-the-centroid-of-a-polygon

qreal Polygon::area() const
{
    qreal area (0.0);
    int N (count ());

    if (N < 3)
    {
        return area;
    }

    for (int i = 0; i < N - 1; ++i)
    {
        const Point &p0 ((*this) [i]);
        const Point &p1 ((*this) [(i + 1) % N]);
        area += p0.x () * p1.y () - p1.x () * p0.y ();
    }

    area /= 2.0;

    return area;
}

const Point Polygon::centroid() const
{
    Point centroid;
    qreal x (NAN);
    qreal y (NAN);
    qreal area (0.0);
    int N (count ());

    if (N < 3)
    {
        return centroid;
    }

    x = 0.0;
    y = 0.0;

    for (int i=0; i < N - 1; ++i)
    {
        const Point &p0 ((*this) [i]);
        const Point &p1 ((*this) [(i + 1) % N]);
        qreal a = p0.x () * p1.y () - p1.x () * p0.y ();
        area += a;
        x += (p0.x () + p1.x ()) * a;
        y += (p0.x () + p1.y ()) * a;
    }

    area /= 2.0;
    x /= (6.0 * area);
    y /= (6.0 * area);

    centroid.setX (x);
    centroid.setY (y);
    centroid.setZ (front ().z ());

    return centroid;
}

const Point Polygon::center() const
{
    return boundary ().center ();
}

const Point Polygon::dimension() const
{
    return boundary ().dimension ();
}

const QString Polygon::string () const
{
    QString line;
    line.append ('[');

    char typeChar;
    switch (type ())
    {
    default:
    case Polygon::Contour :
        typeChar = 'C';
        break;
    case Polygon::Infill :
        typeChar = 'I';
        break;
    case Polygon::Support :
        typeChar = 'S';
        break;
    }
    line.append (typeChar);

    for (const Point &point : *this)
    {
        line.append (point.string ());
    }
    line.append (']');
    return line;
}

bool Polygon::operator <(const Polygon &other) const
{
    return (m_type < other.m_type);
}

QDebug operator << (QDebug dbg, const Polygon &polygon)
{
    char typeChar (0);
    switch (polygon.type ())
    {
    default:
    case Polygon::PolygonType::Contour:
        typeChar = 'C';
        break;
    case Polygon::PolygonType::Infill:
        typeChar = 'I';
        break;
    case Polygon::PolygonType::Support:
        typeChar = 'S';
        break;
    case Polygon::PolygonType::Extra:
        typeChar = 'E';
        break;
    }

    dbg.nospace () << '[';
    dbg << typeChar;
    for (const Point &point : polygon)
    {
        dbg << point;
    }
    dbg << ']';
    return dbg.space ();
}

QDataStream &operator << (QDataStream &stream, const Polygon &polygon)
{
    stream << polygon.type();
    stream << *((QVector<Point>*)&polygon);
    return stream;
}

QDataStream &operator >> (QDataStream &stream, Polygon &polygon)
{
    int type(0);
    stream >> type;
    stream >> *((QVector<Point>*)&polygon);
    polygon.setType((Polygon::PolygonType)type);
    return stream;
}
