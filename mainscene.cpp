#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QGraphicsItem>
#include <QCursor>
#include <QJsonArray>
#include <QJsonObject>
#include <QGraphicsView>
#include <QRandomGenerator>

#include "mainscene.h"
#include "mainwindow.h"
#include "networkline.h"
#include "routingtable.h"

MainScene::MainScene(QObject *parent) : QGraphicsScene(parent)
{
    connect(this,&MainScene::changed,this,&MainScene::updateAll);
    connect(this,&MainScene::setCounterLabel, qobject_cast<MainWindow*>(global->m_main_window), &MainWindow::setAlgoCounter);
    connect(this,&MainScene::generatePathsList, qobject_cast<MainWindow*>(global->m_main_window), &MainWindow::generatesPathsList);
}

void MainScene::setNodesSelectable(bool state)
{
    if(state){
        for(auto& node: networkNodes){
            node->setFlag(QGraphicsItem::ItemIsSelectable);
        }
    } else {
        for(auto& node: networkNodes){
            //hz
        }
    }
}

void MainScene::writeSceneJson(QJsonObject &jsonObj)
{
    QJsonArray nodes;
    foreach(const NetworkNode *node , networkNodes){
        QJsonObject nodeObject;
        node->writeJson(nodeObject);
        nodes.append(nodeObject);
    }
    QJsonArray lines;
    foreach(const NetworkLine *line , networkLines){
        QJsonObject lineObject;
        line->writeJson(lineObject);
        lines.append(lineObject);
    }
    jsonObj["nodes"] = nodes;
    jsonObj["lines"] = lines;
}

void MainScene::initializeFromJson(QJsonObject &jsonObj)
{
    clear();
    QJsonArray nodes = jsonObj["nodes"].toArray();
    networkNodes.clear();
    foreach (QJsonValue jsonValue, nodes){
       QJsonObject nodeObj = jsonValue.toObject();
       generateNodeFromJson(nodeObj);
    }
    max_id = 0;
    foreach (const NetworkNode *node, networkNodes){
        if(node->getId() > max_id){
            max_id = node->getId() + 1;
        }
    }
    QJsonArray lines = jsonObj["lines"].toArray();
    networkLines.clear();
    feelIdtoNodeMap();
    foreach (QJsonValue jsonValue, lines){
        QJsonObject lineObj = jsonValue.toObject();
        int node1_id = lineObj["node1_id"].toInt();
        int node2_id = lineObj["node2_id"].toInt();
        int weight = lineObj["weight"].toInt();
        NetworkNode *node1 = idToNodeMap[node1_id];
        NetworkNode *node2 = idToNodeMap[node2_id];
        NetworkLine *line = new NetworkLine(node1,node2,weight);
        node1->connectLine(line);
        node2->connectLine(line);
        networkLines.append(line);
        addItem(line);
    }
}

void MainScene::setWeightMode(bool isAuto)
{
    isAutoWeight = isAuto;
}

void MainScene::setManualWeight(int weight)
{
    manual_weight = weight;
}

void MainScene::adjustNodeIds()
{
    std::sort(networkNodes.begin(),networkNodes.end(),
              [](const NetworkNode* node1, const NetworkNode* node2){
                    if(node1->getId() < node2->getId())
                        return true;
                    return false;

    });
    int currentId = 0;
    for(auto& node: networkNodes){
        if(currentId != node->getId()){
            node->setId(currentId);
        }
        currentId++;
    }
    max_id = currentId;
}

void MainScene::removeNode(NetworkNode *node)
{
    removeItem(node);
    networkNodes.removeOne(node);
}

void MainScene::removeLine(NetworkLine *line, NetworkNode* node)
{
    if(node == nullptr){
        line->detachFromNodes();
    } else {
        line->detachFromOtherNode(node);
    }

    removeItem(line);
    networkLines.removeOne(line);
}

void MainScene::showNodeInfo(NetworkNode *node)
{
    emit showNode(node);
}

bool MainScene::algoStep()
{
    int wasChanged = 0;
    for(auto& node : networkNodes){
        wasChanged += static_cast<int>(node->routingAlgorithmStep());
    }
    algo_steps_counter++;
    emit setCounterLabel(algo_steps_counter);
    return static_cast<bool>(wasChanged);
}

void MainScene::endRoutingAlgo()
{
    while(algoStep()){
        qDebug() << "Was Algo Step";
    };
    qDebug() << "End of routingAlgo";
}

bool MainScene::existsNode(int node)
{
    feelIdtoNodeMap();
    return idToNodeMap.contains(node);
}

void MainScene::getShortestPaths(int fromNode, int destNode)
{
    networkPaths.clear();
    feelIdtoNodeMap();
    QList<PathElement*> path;

    std::function<void(int)> generatePathElem = [&](int currentNode) -> void{
        if(currentNode == destNode){
            NetworkNode* dest_node = idToNodeMap[currentNode];
            PathElement* pathElem = new PathElement();
            pathElem->node = dest_node;
            pathElem->line = nullptr;
            path.append(pathElem);
            NetworkPath *netw_path = new NetworkPath(path);
            networkPaths.append(netw_path);
            path.removeLast();
        }else {
            NetworkNode *node = idToNodeMap[currentNode];
            auto next_node_lists = node->getRoutingTable()->getNextNodes(destNode);
            for(int next_node_id: next_node_lists){
                NetworkLine* next_line = node->getNetworkLineTo(next_node_id);
                PathElement* pathElem = new PathElement();
                pathElem->node = node;
                pathElem->line = next_line;
                path.append(pathElem);
                generatePathElem(next_node_id);
                path.removeLast();
            }
        }
    };

    generatePathElem(fromNode);
    emit generatePathsList(networkPaths);
}

QList<NetworkPath *> MainScene::getPaths() const
{
    return networkPaths;
}

void MainScene::drawPath(int pathNumber)
{
    for(auto& node: networkNodes){
        node->unsetPathPart();
    }
    for(auto& line: networkLines){
        line->unsetPathPart();
    }
    NetworkPath* netw_path = networkPaths.at(pathNumber);
    auto path = netw_path->getPath();
    for(auto& pathElem: path){
        pathElem->node->setPathPart();
        if(pathElem->line){
            pathElem->line->setPathPart();
        }
    }
}

void MainScene::enableEditMode()
{
    algo_steps_counter = 0;
    networkPaths.clear();
}

void MainScene::feelIdtoNodeMap()
{
    foreach(NetworkNode *node, networkNodes){
        idToNodeMap[node->getId()] = node;
    }
}

int MainScene::getWeight()
{
    if(isAutoWeight){
        return weights[QRandomGenerator::global()->bounded(0,weights.size()-1)];
    } else {
        return manual_weight;
    }
}

void MainScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(isSelect2Node && itemAt(mouseEvent->scenePos(),{}) == nullptr){
       isSelect2Node = false;
       views()[0]->setCursor(Qt::ArrowCursor);
    }else {
        QGraphicsScene::mousePressEvent(mouseEvent);
    }
}

void MainScene::addLine()
{
    if((selectedNode1 != nullptr && selectedNode2 != nullptr ) && (selectedNode1 != selectedNode2)
            && !selectedNode1->isConnnectdToNode(selectedNode2)){
        NetworkLine *line = new NetworkLine(selectedNode1,selectedNode2,getWeight());
        selectedNode1->connectLine(line);
        selectedNode2->connectLine(line);
        networkLines.append(line);
        addItem(line);
    }
}

void MainScene::addNode(const QPointF &point)
{
    NetworkNode *node = new NetworkNode(point ,max_id,false);
    max_id++;
    commonNodeCreationPart(node);
}

void MainScene::generateNodeFromJson(QJsonObject &jsonObj)
{
    NetworkNode *node = new NetworkNode(jsonObj);
    commonNodeCreationPart(node);
}

void MainScene::commonNodeCreationPart(NetworkNode *node)
{
    networkNodes.append(node);
    addItem(node);
    node->setFlag(QGraphicsItem::ItemIsMovable);
    qDebug() << "Network node was added! wiht id = " << node->getId();
}

void MainScene::updateAll(const QList<QRectF> &region)
{
    Q_UNUSED(region);
    for(auto& line : networkLines){
        line->update();
    }
}

void MainScene::selectNode1(NetworkNode *node)
{
    selectedNode1 = node;
    isSelect2Node = true;
    views().first()->setCursor(Qt::CrossCursor);
}

void MainScene::selectNode2(NetworkNode *node)
{
    selectedNode2 = node;
    isSelect2Node = false;
    views()[0]->setCursor(Qt::ArrowCursor);
    addLine();
}

void MainScene::startRoutingAlgorithm(RoutingMetrics rout_metr)
{
    for(auto& node: networkNodes){
        node->startOfRoutingAlgorithm(rout_metr);
    }   
}
