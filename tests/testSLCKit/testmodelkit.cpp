#include <QString>

#include <QElapsedTimer>
#include <iostream>
#include <time.h>
#include "model.h"

int _rand (int max)
{
    std::srand (time (0));
    // range: 1 ~ max
    return std::rand () % (max - 1) + 1;
}

int main (/*int argc, char* argv[]*/)
{
    double a1 = 10.003;
    double a2 = 9.988;
    double a3 = -9.4999;
    double a4 = -9.5000;

    qDebug () << a1 << round(a1);
    qDebug () << a2 << round(a2);
    qDebug () << a3 << round(a3);
    qDebug () << a4 << round(a4);

    Point p1 (10,100,0);
    Point p2 (30,20,0);

    Point p3 = p1 + p2;
    Point p4 = p1 - p2;
    Point p5 = p3 * 10;
    Point p6 = p4 / 10;

    qDebug () << p1;
    qDebug () << p2;
    qDebug () << p3;
    qDebug () << p4;
    qDebug () << p5;
    qDebug () << p6;

    p3 += p4;
    qDebug () << p3;
    p3 /= 0.000000000001;
    qDebug () << p3;
    p3 /= 3;
    qDebug () << p3;

    Polygon c1;
    c1.append(p1);
    c1.append(p2);
    c1.append(p3);
    c1.append(p4);
    c1.append(p5);
    c1.append(p6);

    Polygon c2 (c1);
    c2.reverse();

    Polygon c3 = c2.reversed();

    qDebug () << c1;
    qDebug () << c2;
    qDebug () << c3;

    qDebug () << c3.closed();
    qDebug () << Polygon().boundary ();

    Polygon c4;
    c4.append (Point (0, 0, 0));
    c4.append (Point (0, 90, 0));
    c4.append (Point (30, 90, 0));
    c4.append (Point (30, 60, 0));
    c4.append (Point (60, 60, 0));
    c4.append (Point (60, 90, 0));
    c4.append (Point (90, 90, 0));
    c4.append (Point (90, 0, 0));
    qDebug () << c4.area () << c4.centroid () << c4.center () << c4.dimension ();
    c4.close ();
    qDebug () << c4.area () << c4.centroid () << c4.center () << c4.dimension ();

    Layer layer;
    layer.append(c2);
    layer.append(c3);

    Layer layer2;
    Polygon ca1;
    ca1.append (Point (0, 0));
    ca1.append (Point (0,100));
    ca1.append (Point (100, 100));
    ca1.append (Point (100, 0));

    ca1.reverse ();

    Polygon ca2;
    ca2.append (Point (0, 0));
    ca2.append (Point (0,50));
    ca2.append (Point (50, 50));
    ca2.append (Point (50, 0));

    layer2.append (ca1);
    layer2.append (ca2);

    qDebug () << "area test:" << layer2.area ();

    qDebug () << layer;
    qDebug () << layer.boundary ();

    Polygon c5;
    const unsigned int count = 8000000;
    {
        QElapsedTimer t;
        t.start ();

        c5.reserve(count);
        qDebug () << "elasped time:" << double (t.nsecsElapsed ())/1e9;
        for (unsigned int i = 0; i < count; ++i)
        {
            c5.append(Point(rand(), rand(), rand()));
        }
        layer.append(c5);
        qDebug () << "elasped time:" << double (t.nsecsElapsed ())/1e9;
        qDebug () << c5.size();
        qDebug () << layer.boundary ();
        qDebug () << "elasped time:" << double (t.nsecsElapsed ())/1e9;
    }
    c5.clear ();
    c5.squeeze ();

    QElapsedTimer t;
    t.start ();
    unsigned int contourCountMax = 100;
    unsigned int pointCountMax = 100;
    unsigned int points = 0;
    Model m;
    m.reserve (100);
    for (unsigned int layerIndex = 0;
         layerIndex < 100;
         ++ layerIndex)
    {
        unsigned int contourCount = _rand (contourCountMax);
        Layer l;
        l.reserve (contourCount);
        for (unsigned int contourIndex = 0;
             contourIndex < contourCount;
             ++ contourIndex)
        {
            unsigned int pointCount = _rand (pointCountMax);
            Polygon c;
            c.reserve (pointCount);
            for (unsigned int pointIndex = 0;
                 pointIndex < pointCount;
                 ++ pointIndex)
            {
                Point p(rand(), rand(), rand());
                c.append (p);
                points ++;
            }
            l.append (c);
        }
        m.append (l);
    }
    qDebug () << m.front ().boundary ();
    qDebug () << "elasped time:" << double (t.nsecsElapsed ())/1e9;
    qDebug () << points << "points inserted to model";

    Polygon redundant;
    redundant.append (Point (0, 0, 0));
    redundant.append (Point (50, 50, 0));
    redundant.append (Point (100, 100, 0));
    redundant.append (Point (100, 100, 0));
    redundant.append (Point (100, 100, 0));
    redundant.append (Point (100, 100, 0));
    redundant.append (Point (200, 100, 0));
    redundant.append (Point (300, 100, 0));
    redundant.append (Point (350, 100, 0));
    redundant.append (Point (400, 100, 0));

    qDebug () << redundant;
    qDebug () << redundant.simplified ();

    Layer l;
    Polygon p;

    const int iterationCount = 10;
    for (int i = 0; i < iterationCount; ++i)
    {
        p.append (Point (100 + 100 * cos (double (i) * 2 * PI / iterationCount), 100 + 100 * sin (double (i) * 2 * PI / iterationCount), 100));
    }
    l.append (p);

    t.restart ();

    Layer o (l);
    for (Polygon &polygon : o)
    {
        polygon.simplify ();
    }
    qDebug () << o << (o == l);

    qDebug () << "time elapsed:" << double (t.nsecsElapsed ()) / 1e9 << "s";

    Layer layerSort;
    Polygon p11;
    p11.append (Point (1.0, 1.0, 1.0));
    Polygon p12;
    p12.append (Point (2.0, 2.0, 2.0));

    layerSort.append (p11);
    layerSort.append (Polygon ());
    layerSort.append (Polygon ());
    layerSort.append (p12);

    layerSort[0].setType (Polygon::PolygonType::Infill);
    layerSort[1].setType (Polygon::PolygonType::Support);
    layerSort[2].setType (Polygon::PolygonType::Contour);
    layerSort[3].setType (Polygon::PolygonType::Infill);

    qDebug () << layerSort;
    qDebug () << layerSort.sorted (Layer::SortPattern::SupportInfillContour);

    qDebug () << "TEST FINISHED.";
    std::cin.get ();
    return 0;
}
