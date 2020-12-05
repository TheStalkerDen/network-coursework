#include "networknode.h"
#include "routingtablewidget.h"

#include <QPainter>
#include <QDebug>
#include <QIcon>
#include <QMenu>
#include <QGraphicsSceneMouseEvent>
#include <QRandomGenerator>
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
    setZValue(1);
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
    setZValue(1);
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

void NetworkNode::addToInBufferPackageFromNode(int fromNode, NetworkPackage *package)
{
    package->addToPackageLog(QString("package was succesfully sent from node %1 to in_buffer of node %2 at tick %3\n").arg(fromNode).arg(id_).arg(global->simData.tick_count));
    package->setPackageStatus(PackageStatus::InBuff);
    package->setPos(scenePos());
    package->update();
    nodeToLine[fromNode]->buffers.in_buf.append(package);
}

void NetworkNode::simulationTick()
{
    qDebug() << "simulation tick in node" << id_;
    for(auto& detail: nodeToLine){
        processConnLineDetailsInBuffer(detail);
    }

    if(isSendNode){
        qDebug() << "started send node special process";
        if(global->sendingType == SendingType::LogicalConnection || global->sendingType == SendingType::VirualChannel){
            if(sendNextPackage){
                auto* package = shouldSend->dequeue();
                int nextNode = calculateNextNode(package);
                package->setPackageStatus(PackageStatus::OutBuff);
                package->setVisible(true);
                nodeToLine[nextNode]->buffers.out_buf.enqueue(package);
                sendNextPackage = false;
            }
        } else if (global->sendingType == SendingType::Datagram){
            if(!shouldSend->isEmpty()){
                auto* package = shouldSend->dequeue();
                int nextNode = calculateNextNode(package);
                package->setPackageStatus(PackageStatus::OutBuff);
                package->setVisible(true);
                nodeToLine[nextNode]->buffers.out_buf.enqueue(package);
            }
        }
    }
    processPackagesInNode();

    for(auto& detail: nodeToLine){
        processConnLineDetailsOutBuffer(detail);
    }
}

void NetworkNode::processConnLineDetailsInBuffer(ConnectedLineDetails *detail)
{    
    qDebug() << "process connn line details IN BUFFER for node"  << detail->connectedNode->getId();
    auto processPackageInDestNode = [&](NetworkPackage* package){
        package->addToPackageLog(QString("package successfully arrive to destination at tick %1\n").arg(global->simData.tick_count));
        package->setPackageStatus(PackageStatus::Sent);
        package->setVisible(false);
        PathElement *pathElem = new PathElement();
        pathElem->line = nullptr;
        pathElem->node = this;
        package->addPathElement(pathElem);
        global->simData.packages_on_scene.removeOne(package);
        global->main_scene->removeItem(package);
        switch (package->getType()) {
            case PackageType::Service:
                switch (package->getPackageServiceType()){
                    case PackageServiceType::EnqConn: {
                        NetworkPackage *confConn = new NetworkPackage("ConfConn",package->getId()+1, id_,package->getFrom_node(),PackageType::Service);
                        confConn->setPackageServiceType(PackageServiceType::ConfConn);
                        confConn->setPackageStatus(PackageStatus::Processing);
                        confConn->setHeader_size(package->getHeader_size());
                        global->simData.all_packages.append(confConn);
                        global->simData.packages_on_scene.append(confConn);
                        global->main_scene->addItem(confConn);
                        packageToProcess.append(confConn);
                        break;
                     }
                    case PackageServiceType::ConfConn: {
                        sendNextPackage = true;
                        break;
                    }
                    case PackageServiceType::PackageConfirmation: {
                        sendNextPackage = true;
                        break;
                    }
                    case PackageServiceType::EnqDis: {
                        NetworkPackage *enqDis = new NetworkPackage("ConfDis",package->getId()+1, id_,package->getFrom_node(),PackageType::Service);
                        enqDis->setPackageServiceType(PackageServiceType::ConfDis);
                        enqDis->setPackageStatus(PackageStatus::Processing);
                        enqDis->setHeader_size(package->getHeader_size());
                        global->simData.all_packages.append(enqDis);
                        global->simData.packages_on_scene.append(enqDis);
                        global->main_scene->addItem(enqDis);
                        packageToProcess.append(enqDis);
                        break;
                    }
                    case PackageServiceType::ConfDis: {
                        break;
                    }
                }
                break;
            case PackageType::Info:
                switch (global->sendingType) {
                    case SendingType::Datagram:
                        break;
                    case SendingType::LogicalConnection:
                    case SendingType::VirualChannel: {
                        NetworkPackage *confPackage = new NetworkPackage("ConfPackege",package->getId()+1, id_,package->getFrom_node(),PackageType::Service);
                        confPackage->setPackageServiceType(PackageServiceType::PackageConfirmation);
                        confPackage->setPackageStatus(PackageStatus::Processing);
                        confPackage->setHeader_size(package->getHeader_size());
                        global->simData.all_packages.append(confPackage);
                        global->simData.packages_on_scene.append(confPackage);
                        global->main_scene->addItem(confPackage);
                        packageToProcess.append(confPackage);
                        break;
                    }
                }
        }
    };

    auto processPackageInTransferNode = [&](NetworkPackage* package){
        package->addToPackageLog(QString("Package arrive to %1 node\n").arg(id_));
        packageToProcess.append(package);
    };

    QMutableListIterator<NetworkPackage*> itr(detail->buffers.in_buf);

    while(itr.hasNext()){
        itr.next();
        auto package = itr.value();
        if(package->getDest_node() == id_){
            processPackageInDestNode(package);
        } else {
            processPackageInTransferNode(package);
        }
        itr.remove();
    }
}

void NetworkNode::processPackagesInNode()
{
    qDebug() << "start processing packages in node: " << id_;
    QMutableListIterator<NetworkPackage*> itr(packageToProcess);
    while(itr.hasNext()){
        itr.next();
        auto& package = itr.value();
        if(package->getPackageStatus() == PackageStatus::InBuff){
            package->setPackageStatus(PackageStatus::Processing);
        } else if(package->getPackageStatus() == PackageStatus::Processing){
            package->addToPackageLog(QString("Pakage is processing in node %1. (tick = %2)").arg(id_).arg(global->simData.tick_count));
            package->setVisible(true);
            package->update();
            package->setTTL(package->getTTL() -1);
            if(package->getTTL() <= 0){
                qDebug() << "TTL is bad";
                package->addToPackageLog(QString("TTL is zero. Package was killed! (tick = %1)").arg(global->simData.tick_count));
                package->setPackageStatus(PackageStatus::Killed);
                global->simData.packages_on_scene.removeAll(package);
                itr.remove();
            }
            int nextNode = calculateNextNode(package);
            package->setPackageStatus(PackageStatus::OutBuff);
            package->addToPackageLog(QString("Package was moved to OUT buffer to node %1. (tick = %2)").arg(nextNode).arg(global->simData.tick_count));
            nodeToLine[nextNode]->buffers.out_buf.enqueue(package);
            itr.remove();
        }
    }
    qDebug() << "after processing packages in node" << id_;
}

void NetworkNode::processConnLineDetailsOutBuffer(ConnectedLineDetails *details)
{
    qDebug() << "start processing in OUT BUFFER of " << details->connectedNode->getId();
    int canBeSentBytes = details->line->bytesPerTick();

    auto generatePathElement = [&](){
        PathElement *pathElem = new PathElement();
        pathElem->line = details->line;
        pathElem->node = this;
        return pathElem;
    };

    auto defaultTransferActions = [&](NetworkPackage* currentPackage){
        if(currentPackage->getBytesToSentThrougLine() > canBeSentBytes){
            currentPackage->setBytesToSentThrougLine(currentPackage->getBytesToSentThrougLine() - canBeSentBytes);
            currentPackage->setPosition(this,details->connectedNode);
            canBeSentBytes = 0;
        } else if  (currentPackage->getBytesToSentThrougLine() == canBeSentBytes){
            if(QRandomGenerator::global()->bounded(0,100) < details->line->getError_possibility()){
                currentPackage->addToPackageLog(QString("Error transfer. Will be transfered again. (tick = %1)\n").arg(global->simData.tick_count));
                currentPackage->setBytesToSentThrougLine(currentPackage->getPackageSize());
                currentPackage->setPosition(this,details->connectedNode);
            }else {
                details->buffers.out_buf.dequeue();
                currentPackage->setBytesToSentThrougLine(currentPackage->getBytesToSentThrougLine() - canBeSentBytes);
                currentPackage->setPosition(this,details->connectedNode);
                currentPackage->addPathElement(generatePathElement());
                details->connectedNode->addToInBufferPackageFromNode(id_, currentPackage);
                canBeSentBytes = 0;
            }
        } else {
            if(QRandomGenerator::global()->bounded(0,100) < details->line->getError_possibility()){
                currentPackage->addToPackageLog(QString("Error transfer. Will be transfered again. (tick = %1)\n").arg(global->simData.tick_count));
                currentPackage->setBytesToSentThrougLine(currentPackage->getPackageSize());
                currentPackage->setPosition(this,details->connectedNode);
            } else {
                canBeSentBytes -= currentPackage->getBytesToSentThrougLine();
                details->buffers.out_buf.dequeue();
                currentPackage->setBytesToSentThrougLine(0);
                currentPackage->setPosition(this,details->connectedNode);
                currentPackage->addPathElement(generatePathElement());
                details->connectedNode->addToInBufferPackageFromNode(id_, currentPackage);
            }
        }
    };


    if(details->buffers.out_buf.isEmpty()){
        qDebug() << "out buffer is empty";
        return;
    }
    while(!details->buffers.out_buf.isEmpty() && canBeSentBytes){
        qDebug() << "In while cicle";
        auto* currentPackage = details->buffers.out_buf.head();
        if(currentPackage->getPackageStatus() == PackageStatus::OutBuff){
            qDebug() << "package status is OutBuff";
            currentPackage->setPackageStatus(PackageStatus::Sending);
            currentPackage->setBytesToSentThrougLine(currentPackage->getPackageSize());
            defaultTransferActions(currentPackage);
        } else if(currentPackage->getPackageStatus() == PackageStatus::Sending){
            qDebug() << "package status is Sending";
            defaultTransferActions(currentPackage);
        } else {
            qDebug() << "was catched package status with name";
            details->buffers.out_buf.dequeue();
        }
    }
    qDebug() << "after processing in OUT BUFFER of " << details->connectedNode->getId();

}

int NetworkNode::calculateNextNode(NetworkPackage *package)
{
    auto algNextFreeLine = [&](){
        //BETTERISE
        return current_routing_table->getNextNodes(package->getDest_node())[0];
    };

    auto algOnlyBestWay = [&](){
        return current_routing_table->getNextNodes(package->getDest_node())[0];
    };

    switch(global->sendingType){
        case SendingType::Datagram:
        case SendingType::LogicalConnection:
            return algNextFreeLine();
        case SendingType::VirualChannel:
            return algOnlyBestWay();
    }
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

void NetworkNode::initBeforeSimulation()
{
    isSendNode = false;
    nodeToLine.clear();
    for(auto& line : connectedLines){
        ConnectedLineDetails *details = new ConnectedLineDetails;
        details->connectedNode = line->returnOtherNode(this);
        details->buffers = {};
        details->line = line;
        nodeToLine[details->connectedNode->getId()] = details;
    }
}

void NetworkNode::setShouldSent(QQueue<NetworkPackage *> *shouldSend)
{
    this->shouldSend = shouldSend;
}

QList<NetworkLine *> NetworkNode::getConnectedLines() const
{
    return connectedLines;
}

void NetworkNode::setConnectedLines(const QList<NetworkLine *> &value)
{
    connectedLines = value;
}

bool NetworkNode::getHasConnectedHost() const
{
    return hasConnectedHost_;
}

void NetworkNode::setHasConnectedHost(bool hasConnectedHost)
{
    hasConnectedHost_ = hasConnectedHost;
}

bool NetworkNode::getIsSendNode() const
{
    return isSendNode;
}

void NetworkNode::setIsSendNode(bool value)
{
    isSendNode = value;
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
