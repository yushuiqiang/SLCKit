#include "model.h"
#include <QFile>
#include <QFileInfo>
#include <QDataStream>

#ifdef USE_COMPRESSION
#include "kcompressiondevice.h"
#endif

const Boundary Model::boundary() const
{
    Boundary boundary;
    for (const Layer &layer : *this)
    {
        boundary.refer (layer.boundary ());
    }
    return boundary;
}

const Point Model::center() const
{
    return boundary ().center ();
}

const Point Model::dimension() const
{
    return boundary ().dimension ();
}

const QList<qreal> Model::heights() const
{
    return m_heights;
}

const Layer Model::at (int index) const
{
    if (index >= 0 && index < count ())
        return QVector::at (index);
    else
        return Layer ();
}

const Layer Model::layerAtHeight(const qreal height) const
{
    int index = m_heights.indexOf (height);
    return at (index);
}

qreal Model::contourStartHeight() const
{
    qreal startHeight (NAN);
    int index (contourStartIndex ());
    if (index >= 0)
    {
        startHeight = m_heights.at (index);
    }
    return startHeight;
}

int Model::contourStartIndex() const
{
    int index (-1);

    for (int i = 0; i < count (); ++ i)
    {
        const Layer &layer ((*this) [i]);
        for (int j = 0; j < layer.count (); ++j)
        {
            const Polygon &polygon (layer [j]);
            Polygon::PolygonType type = polygon.type ();
            if(type != Polygon::PolygonType::Support)
            {
                index = i;
                break;
            }
        }

        if (index >= 0)
        {
            break;
        }
    }

    return index;
}

void Model::sort()
{
    m_heights.clear ();
    std::stable_sort (this->begin (), this->end ());

    for (const Layer &layer : *this)
    {
        m_heights.append (layer.height ());
    }
}

void Model::merge(const Model &other)
{
    sort ();

    for (const Layer &layer : other)
    {
        qreal height = layer.height ();
        if (m_heights.contains (height))
        {
            int index = m_heights.indexOf (height);
            operator [] (index) += layer;
        }
        else
        {
            append (layer);
        }
    }

    sort ();
}

const Model Model::readSLC(const QString &filename)
{
    bool ok (false);
    Model model;
    do
    {
        if (filename.isEmpty ())
        {
            break;
        }

        QFile slcFile(filename);
        model.setName (QFileInfo (slcFile).baseName ());

        if(! slcFile.open(QIODevice::ReadOnly) )
        {
            // qDebug()<<"Open SLC file error:" << filename;
            break;
        }

        QDataStream dataStream(&slcFile);
        dataStream.setByteOrder(QDataStream::LittleEndian);
        dataStream.setFloatingPointPrecision(QDataStream::SinglePrecision);

        /****************************************************************/
        /*-----------------------header section-------------------------*/
        /****************************************************************/
        qint8 headerChar;
        QByteArray headerData;
        while(!dataStream.atEnd() && dataStream.status() == QDataStream::Status::Ok)
        {
            dataStream >> headerChar;
            if(headerChar == 0x0d)
            {
                dataStream >> headerChar;
                if(headerChar == 0x0a)
                {
                    dataStream >> headerChar;
                    if(headerChar == 0x1a)
                    {
                        // break header reading
                        break;
                    }
                }
            }
            headerData.append (headerChar);
        }

        // split header with whitespace
        QList <QByteArray> headerList (headerData.split (' '));

        // extract SLCVER, UNIT, TYPE info in the header
        int versionIndex (headerList.indexOf ("-SLCVER"));
        int unitIndex (headerList.indexOf ("-UNIT"));
        int typeIndex (headerList.indexOf ("-TYPE"));

        if (versionIndex < 0 || unitIndex < 0 || typeIndex < 0)
        {
            // break loading
            break;
        }

        QByteArray versionString (headerList.at (versionIndex + 1));
        QByteArray unitString (headerList.at (unitIndex + 1));
        QByteArray typeString (headerList.at (typeIndex + 1));

        // validate header info

        // requires SLCVER >= 2.0 to proceed
        // since i never know other version there :P
        if (versionString.toDouble () < 2.0)
        {
            // break loading
            break;
        }

        // the unit might be INCH in some case.
        // then a unitscale was multiplied
        // which means the internal unit shall always be MM
        qreal unitScale (1.0);
        if (unitString != "MM")
        {
            unitScale = 2.54;
        }

        // in case the model file specified was a support type part
        // the polygons of layers of this model should be of Support type.
        bool isPart (false);
        if (typeString == "PART")
        {
            isPart = true;
        }

        /****************************************************************/
        /*-----------------------reserve section------------------------*/
        /****************************************************************/
        dataStream.skipRawData(256);

        /****************************************************************/
        /*-----------------------sampling table*------------------------*/
        /****************************************************************/
        quint8 samplingTableSize;
        dataStream >> samplingTableSize;
        for(quint8 i=0; i<samplingTableSize; i++)
        {
            /*
                Minimum Z Level             1 Float
                Layer Thickness             1 Float
                Line Width Compensation     1 Float
                Reserved                    1 Float
            */
            dataStream.skipRawData(sizeof (float) * 4);    //1 float = 4 byte
        }

        /****************************************************************/
        /*-----------------------contour data---------------------------*/
        /****************************************************************/
        /*
            Minimum Z Layer                                 float
            Number of Boundaries                            unsigned int 4byte

            Number of Vertices for the 1st Boundary         unsigned int
            Number of Gaps     for the 1st Boundary         unsigned int
            Vertex List for 1st Boundary                    (x/y) 2float

            Number of Vertices for the 2st Boundary
            Number of Gaps     for the 2st Boundary
            Vertex List        for 2st Boundary
         */
        while(!dataStream.atEnd() && dataStream.status () == QDataStream::Status::Ok)
        {
            float minZlevel;
            quint32 numberOfBoundary;
            dataStream >> minZlevel;
            dataStream >> numberOfBoundary;
            //judge for termination
            if( numberOfBoundary == 0XFFFFFFFF )
            {
                ok = true;
                Q_UNUSED (ok);
                // qDebug() << "Reading SLC file finished.";
                break;
            }
            //process the new layer
            Layer layer;
            layer.reserve (numberOfBoundary);

            quint32 numberOfVertices , numberOfGaps;
            //process every polygon in this layer
            for( quint32 boundaryId = 0; boundaryId < numberOfBoundary ; boundaryId++ )
            {
                Polygon polygon;
                dataStream >> numberOfVertices;
                dataStream >> numberOfGaps;
                polygon.reserve (numberOfVertices);

                float verticeX, verticsY;
                for( quint32 verticeId = 0; verticeId <numberOfVertices; verticeId ++)
                {
                    dataStream >> verticeX >> verticsY;
                    Point point(verticeX * unitScale, verticsY * unitScale);
                    polygon.append( point );
                }

                if (isPart)
                {
                    polygon.setType (Polygon::PolygonType::Contour);
                }
                else
                {
                    polygon.setType (Polygon::PolygonType::Support);
                }

                layer.append(polygon);
            }
            layer.setHeight (minZlevel * unitScale);
            model.append(layer);
        }
        slcFile.close();

        model.sort ();
    }
    while (false);
    return model;
}

void Model::translate(const Point &offset)
{
    for (Layer &layer : *this)
    {
        layer.translate (offset);
    }
}

const Model Model::translated(const Point &offset) const
{
    Model other (*this);
    other.translate (offset);
    return other;
}

const Model Model::readXLC(const QString &filename)
{
    Model model;
    do
    {

#ifdef USE_COMPRESSION
        KCompressionDevice device (filename, KCompressionDevice::CompressionType::GZip);
#else
        QFile device (filename);
#endif

        if (! device.open (QIODevice::ReadOnly))
        {
            break;
        }

        QDataStream stream (&device);
        QString version;
        stream >> version;

        if (version != QStringLiteral ("XLC v2.0"))
        {
            device.close ();
            break;
        }

        stream >> model;

        device.close ();
    }
    while (false);
    return model;
}

bool Model::saveXLC(const QString &filename) const
{
    bool ok (false);
    do
    {

#ifdef USE_COMPRESSION
        KCompressionDevice device (filename, KCompressionDevice::CompressionType::GZip);
#else
        QFile device (filename);
#endif

        if (! device.open (QIODevice::WriteOnly))
        {
            break;
        }

        QDataStream stream (&device);
        stream << QStringLiteral ("XLC v2.0");

        stream << (*this);

        device.close ();

        ok = true;
    }
    while (false);
    return ok;
}

const QString Model::name() const
{
    return m_name;
}

void Model::setName(const QString &name)
{
    m_name = name;
}

QDataStream &operator <<(QDataStream &stream, const Model &model)
{
    stream << model.name ();
    stream << *((QVector<Layer>*)&model);
    return stream;
}

QDataStream &operator >>(QDataStream &stream, Model &model)
{
    QString name;
    stream >> name;
    stream >> *((QVector<Layer>*)&model);
    model.setName (name);
    model.sort ();
    return stream;
}

