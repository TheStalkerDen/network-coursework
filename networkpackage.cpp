#include "networknode.h"
#include "networkpackage.h"

#include <QPainter>
#include <QDebug>


NetworkPackage::NetworkPackage(QString packageName, int id, int from_node, int dest_node, PackageType type, QGraphicsObject *parent)
    :QGraphicsObject(parent),type(type),packageName(packageName),id(id),dest_node(dest_node),from_node(from_node)
{
    qDebug() << "network package was created";
    setZValue(4);
}

QString NetworkPackage::getPackageName() const
{
    return packageName;
}

void NetworkPackage::setPackageName(const QString &value)
{
    packageName = value;
}

void NetworkPackage::setPosition(NetworkNode *node1, NetworkNode *node2)
{
    auto pos1 = node1->scenePos();
    auto pos2 = node2->scenePos();

    auto point = pos2 - pos1;
    double coeff = 1 -  (double)bytesToSentThrougLine/getPackageSize();
    auto newpos = pos1 + point*coeff;
    setPos(newpos);
    update();
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

void NetworkPackage::addToPackageLog(QString info)
{
    packageLog.append(info);
}

void NetworkPackage::addPathElement(PathElement *pathElement)
{
    packagePath->addToPath(pathElement);
}

int NetworkPackage::getPackageSize()
{
    return header_size + data_size;
}

int NetworkPackage::getBytesToSentThrougLine() const
{
    return bytesToSentThrougLine;
}

void NetworkPackage::setBytesToSentThrougLine(int value)
{
    bytesToSentThrougLine = value;
}

int NetworkPackage::getDest_node() const
{
    return dest_node;
}

void NetworkPackage::setDest_node(int value)
{
    dest_node = value;
}

int NetworkPackage::getFrom_node() const
{
    return from_node;
}

void NetworkPackage::setFrom_node(int value)
{
    from_node = value;
}

int NetworkPackage::getTTL() const
{
    return TTL;
}

void NetworkPackage::setTTL(int value)
{
    TTL = value;
}

PackageStatus NetworkPackage::getPackageStatus() const
{
    return packageStatus;
}

PackageServiceType NetworkPackage::getPackageServiceType() const
{
    return packageServiceType;
}

void NetworkPackage::setPackageServiceType(const PackageServiceType &value)
{
    packageServiceType = value;
}

void NetworkPackage::setPackageStatus(const PackageStatus &value)
{
    packageStatus = value;
}

NetworkPath *NetworkPackage::getPackagePath() const
{
    return packagePath;
}

PackageType NetworkPackage::getType() const
{
    return type;
}

QString NetworkPackage::getPackageLog() const
{
    return packageLog;
}


QRectF NetworkPackage::boundingRect() const
{
    return QRectF(-40,-50,+80,+100);
}

void NetworkPackage::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(QColor(Qt::red));
    painter->drawText(QPointF(0,0) ,packageName);
}
