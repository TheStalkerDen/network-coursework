#include "networknode.h"
#include "routingtablewidget.h"

#include <QPainter>
#include <QDebug>
#include <QIcon>
#include <QMenu>
#include <QGraphicsSceneMouseEvent>
#include <QJsonObject>
#include <mainscene.h>
#include <mainwindow.h>
#include <routingtable.h>
#include "networkline.h"

NetworkNode::NetworkNode(QPointF position, int id, bool hasConnectedHost, QGraphicsObject *parent)
    :QGraphicsObject(parent), id_(id), hasConnectedHost_(hasConnectedHost)
{
    qDebug() << "id = " << this->id_;
    setPos(position);
}

NetworkNode::NetworkNode(QJsonObject &jsonObj, QGraphicsObject *parent)
    :QGraphicsObject(parent)
{
    id_ = jsonObj["id"].toInt();
    hasConnectedHost_ = jsonObj["connectedHost"].toBool();
    QPointF pos;
    pos.setX(jsonObj["x_pos"].toInt());
    pos.setY(jsonObj["y_pos"].toInt());
    setPos(pos);
    m_scene = dynamic_cast<MainScene*>(scene());
}

void NetworkNode::writeJson(QJsonObject &jsonObj) const
{
    jsonObj["id"] = id_;
    jsonObj["connectedHost"] = hasConnectedHost_;
    jsonObj["x_pos"] = pos().x();
    jsonObj["y_pos"] = pos().y();
}

void NetworkNode::connectLine(NetworkLine *line)
{
    connectedLines.append(line);
}

void NetworkNode::removeLine(NetworkLine *line)
{
    connectedLines.removeOne(line);
}

bool NetworkNode::isConnnectdToNode(NetworkNode *node)
{
    for(auto& line: connectedLines){
        if(line->returnOtherNode(this) == node){
            return true;
        }
    }
    return false;
}

NetworkLine *NetworkNode::getNetworkLineTo(int nextNode)
{
    for(NetworkLine* line: connectedLines){
        if(line->returnOtherNode(this)->getId() == nextNode){
            return line;
        }
    }
    qDebug() << "On no! Next network line wasn't found!!!";
    return nullptr;
}

void NetworkNode::startOfRoutingAlgorithm(RoutingMetrics rout_metr)
{

    QList<RoutingEntity> init_data;
    for(auto& line: connectedLines){
        RoutingEntity data;
        data.metrics = line->getWeight();
        data.nodes = {line->returnOtherNode(this)->getId()};
        init_data.append(data);
    }
    future_routing_table = new RoutingTable(id_ ,init_data,rout_metr);
    current_routing_table = new RoutingTable(id_, init_data,rout_metr);
}

bool NetworkNode::routingAlgorithmStep()
{
    future_routing_table->setRoutingTable(current_routing_table->getRoutingMap());
    QList<QPair<int,RoutingTable*>> tables_from_other_nodes;
    for(auto& line: connectedLines){
        QPair<int,RoutingTable*> pair;
        NetworkNode* otherNode = line->returnOtherNode(this);
        pair.first = otherNode->getId();
        pair.second = otherNode->getRoutingTable();
        tables_from_other_nodes.append(pair);
    }
    bool wasUpdated = future_routing_table->updateTable(tables_from_other_nodes);
    switchTables();
    return wasUpdated;
}

RoutingTable* NetworkNode::getRoutingTable() const
{
    return current_routing_table;
}

void NetworkNode::switchTables()
{
    current_routing_table->setRoutingTable(future_routing_table->getRoutingMap());
}

void NetworkNode::setPathPart()
{
    isPathPart = true;
    emit update();
}

void NetworkNode::unsetPathPart()
{
    isPathPart = false;
    emit update();
}

QRectF NetworkNode::boundingRect() const
{
    qreal penWidth = 1;
    return QRectF(-20 - penWidth/2, -20 - penWidth/2,
                  40+penWidth,40+penWidth);
}

void NetworkNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(isPathPart){
        painter->setBrush(Qt::red);
    } else if (hasConnectedHost_) {
       painter->setBrush(QBrush(QColor(1,29,33)));
    } else {
        painter->setBrush(QBrush(QColor(100,29,33)));
    }
    painter->setPen(QColor(220,221,0));
    painter->drawEllipse(-20,-20,40,40);
    painter->setPen(QColor(220,0,230));
    painter->drawText(0,0,QString::number(id_));
}

void NetworkNode::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    m_scene = dynamic_cast<MainScene*>(scene());
    if(m_scene->isSelect2Node){
        m_scene->selectNode2(this);
    } else {
        m_scene->showNodeInfo(this);
    }
}

void NetworkNode::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu;

    QAction* action_nodeConnection;
    QAction* action_deleteNode;
    QAction* action_showRoutingTable;


    if(!global->is_simulation){
        action_nodeConnection = menu.addAction(
                    QIcon(),
                    QString("Connect Node to")
                    );
        menu.addSeparator();
        action_deleteNode = menu.addAction(
                    QIcon(),
                    QString("Delete Node")
                    );
    } else {
        action_showRoutingTable = menu.addAction(
                QIcon(),
                QString("Show Routing Table")
                );
    }
    qDebug() << "It is contextEvent for node with id: " << id_;
    m_scene = qobject_cast<MainScene*>(scene());
    QAction* selected_action = menu.exec(event->screenPos());
      if(selected_action) {
        if(selected_action == action_nodeConnection)
        {
            m_scene->selectNode1(this);
        }
        else if(selected_action == action_deleteNode)
        {
            for(auto& line: connectedLines){
                m_scene->removeLine(line,this);
            }
            m_scene->removeNode(this);
            m_scene->adjustNodeIds();
        }
        else if(selected_action == action_showRoutingTable){
            RoutingTableWidget *widget = new RoutingTableWidget(current_routing_table, qobject_cast<QWidget*>(global->m_main_window));
            widget->show();
            widget->setWindowTitle("Routing Table of Node " + QString::number(id_));
        }
      }
}
