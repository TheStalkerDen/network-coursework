#include "mainwindow.h"
#include "networkline.h"
#include "networknode.h"

#include <QPainter>
#include <QDebug>
#include <QJsonObject>
#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>
#include <mainscene.h>


NetworkLine::NetworkLine(NetworkNode *node1, NetworkNode *node2, int weight, QGraphicsObject *parent)
    : QGraphicsObject(parent), weight(weight), node1(node1), node2(node2)
{
    qDebug() << "line was created between: " << node1->getId() << " and " << node2->getId();
    const QLineF lineParm = QLineF(mapFromParent(node1->pos()),mapFromParent(node2->pos()));
    line = new QGraphicsLineItem(this);
    line->setLine(lineParm);
    line->setZValue(-999);
    defaultPen = QPen();
    setPos(calculateMiddlePoint());
    connect(this,&NetworkLine::showLineDetails, qobject_cast<MainWindow*>(global->m_main_window),&MainWindow::showLineDetails);
}

void NetworkLine::writeJson(QJsonObject &jsonObj) const
{
    jsonObj["node1_id"] = node1->getId();
    jsonObj["node2_id"] = node2->getId();
    jsonObj["is_half_duplex"] = isHalfDuplex;
    jsonObj["weight"] = weight;
    jsonObj["error_chance"] = error_possibility;
}

void NetworkLine::detachFromOtherNode(NetworkNode *node)
{
    if (node1 != node){
        node1->removeLine(this);
    }
    else if (node2 != node){
        node2->removeLine(this);
    } else {
        qDebug() << "Incorrect node was passed here for detaching" << node->getId();
    }
}

void NetworkLine::detachFromNodes()
{
    node1->removeLine(this);
    node2->removeLine(this);
}

NetworkNode *NetworkLine::returnOtherNode(NetworkNode *node)
{
    if (node1 != node){
        return node1;
    }
    else if (node2 != node){
        return node2;
    } else {
        qDebug() << "Incorrect node was passed!" << node->getId();
    }
    return nullptr;
}


void NetworkLine::update()
{
    const QLineF lineParm = QLineF(mapFromParent(node1->pos()),mapFromParent(node2->pos()));
    line->setLine(lineParm);
    setPos(calculateMiddlePoint());
}

void NetworkLine::setWeight(int weight)
{
    this->weight = weight;
    emit QGraphicsObject::update();
}

void NetworkLine::setPathPart()
{
    isPathPart = true;
    line->update();
    emit update();
}

void NetworkLine::unsetPathPart()
{
    isPathPart = false;
    line->setPen(defaultPen);
    line->update();
    emit update();
}

QRectF NetworkLine::boundingRect() const
{
    return QRectF(-20,-20,40,40);
}

QPointF NetworkLine::calculateMiddlePoint()
{
    return (node1->pos() + node2->pos())/2;
}

void NetworkLine::setIsSelected(bool value)
{
    isSelected = value;
    emit QGraphicsObject::update();
}

int NetworkLine::getError_possibility() const
{
    return error_possibility;
}

void NetworkLine::setError_possibility(int value)
{
    error_possibility = value;
}

bool NetworkLine::getIsHalfDuplex() const
{
    return isHalfDuplex;
}

void NetworkLine::setIsHalfDuplex(bool value)
{
    isHalfDuplex = value;
    if(isHalfDuplex){
        defaultPen = QPen(QBrush(Qt::black),2,Qt::PenStyle::DashLine);
        line->setPen(defaultPen);
    } else {
        defaultPen = QPen();
        line->setPen(defaultPen);
    }
}

int NetworkLine::bytesPerTick()
{
    return 1000/weight;
}


void NetworkLine::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if(!global->is_simulation){
        QMenu menu;

        QAction* action_firstAction = menu.addAction(
                    QIcon(),
                    QString("Delete line")
                    );
        m_scene = dynamic_cast<MainScene*>(scene());
        QAction* selected_action = menu.exec(event->screenPos());
        if(selected_action){
            if(selected_action == action_firstAction){
                node1->removeLine(this);
                node2->removeLine(this);
                m_scene->removeLine(this);
            }
        }
    }
}

void NetworkLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    if(isSelected){
        painter->setPen(Qt::red);
    } else {
        painter->setPen(Qt::black);
    }
    painter->setFont(QFont("Times",10,QFont::Bold));
    painter->drawText(1,0,QString::number(weight));
    if(isPathPart){
        line->setPen(QPen(QBrush(Qt::red),4));
    }
}


void NetworkLine::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "Line was pressed";
    isSelected = true;
    emit QGraphicsObject::update();
    emit showLineDetails(this);
}
