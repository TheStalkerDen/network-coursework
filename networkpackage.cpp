#include "networkpackage.h"

#include <QPainter>



NetworkPackage::NetworkPackage(QString packageName, int id, PackageType type,  QGraphicsObject *parent)
    :QGraphicsObject(parent), packageName(packageName), id(id), type(type)
{

}

QString NetworkPackage::getPackageName() const
{
    return packageName;
}

void NetworkPackage::setPackageName(const QString &value)
{
    packageName = value;
}

int NetworkPackage::getId() const
{
    return id;
}

void NetworkPackage::setId(int value)
{
    id = value;
}

int NetworkPackage::getHeader_size() const
{
    return header_size;
}

void NetworkPackage::setHeader_size(int value)
{
    header_size = value;
}

int NetworkPackage::getData_size() const
{
    return data_size;
}

void NetworkPackage::setData_size(int value)
{
    data_size = value;
}


QRectF NetworkPackage::boundingRect() const
{
    return QRectF(20,10,-40,-20);
}

void NetworkPackage::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawText(QPointF(0,0) ,packageName);
}
