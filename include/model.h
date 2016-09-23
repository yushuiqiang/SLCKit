#ifndef SLCMODEL_H
#define SLCMODEL_H

#include "layer.h"

class SLCKIT_EXPORT Model :public QVector<Layer>
{
public:
    const Boundary boundary () const;
    const Point center () const;
    const Point dimension () const;

    const QList <qreal> heights () const;
    const Layer at (int index) const;
    const Layer layerAtHeight (const qreal height) const;

    qreal contourStartHeight () const;
    int contourStartIndex () const;

    void sort ();
    void merge(const Model &other);
    static const Model readSLC (const QString &filename);

    void translate (const Point &offset);
    const Model translated (const Point &offset) const;

    static const Model readXLC (const QString &filename);
    bool saveXLC (const QString &filename) const;

    const QString name () const;
    void setName (const QString &name);

private:
    QList<qreal> m_heights;
    QString m_name;
};

SLCKIT_EXPORT QDataStream& operator << (QDataStream &stream, const Model &model);
SLCKIT_EXPORT QDataStream& operator >> (QDataStream &stream, Model &model);

#endif // SLCMODEL_H
