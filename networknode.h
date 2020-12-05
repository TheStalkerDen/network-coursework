#ifndef NETWORKNODE_H
#define NETWORKNODE_H

#include <QContextMenuEvent>
#include <QGraphicsObject>
#include <QList>
#include <QQueue>
#include <QQueue>

#include "global.h"

class NetworkLine;
class MainScene;
enum class RoutingMetrics;
class RoutingTable;
class NetworkNode;

struct LineBuffers{
    QQueue<NetworkPackage*> in_buf;
    QQueue<NetworkPackage*> out_buf;
};

struct ConnectedLineDetails{
    NetworkNode* connectedNode;
    LineBuffers buffers;
    NetworkLine* line;
};

class NetworkNode : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit NetworkNode(QPointF position, int id, bool hasConnectedHost, QGraphicsObject *parent = nullptr);
    explicit NetworkNode(QJsonObject &jsonObj, QGraphicsObject *parent = nullptr);
    void writeJson(QJsonObject &jsonObj) const;
    int getId() const{ return id_;}
    void setId(int id) { id_ = id;}
    void setHostStatus(bool hasHost) { hasConnectedHost_ = hasHost;}
    bool getHostStatus() const { return hasConnectedHost_;}
    void connectLine(NetworkLine* line);
    void removeLine(NetworkLine* line);
    bool isConnnectdToNode(NetworkNode* node);
    NetworkLine* getNetworkLineTo(int nextNode);

    void startOfRoutingAlgorithm(RoutingMetrics rout_metrics);
    void addToInBufferPackageFromNode(int fromNode, NetworkPackage* package);
    void simulationTick();
    void processConnLineDetailsInBuffer(ConnectedLineDetails* details);
    void processPackagesInNode();
    void processConnLineDetailsOutBuffer(ConnectedLineDetails* details);
    int calculateNextNode(NetworkPackage* package);

    bool routingAlgorithmStep();
    RoutingTable* getRoutingTable() const;
    void switchTables();

    void setPathPart();
    void unsetPathPart();

    void initBeforeSimulation();
    void setShouldSent(QQueue<NetworkPackage*> *shouldSend);
    void simulationTic();


    QList<NetworkLine *> getConnectedLines() const;
    void setConnectedLines(const QList<NetworkLine *> &value);

    bool getHasConnectedHost() const;
    void setHasConnectedHost(bool hasConnectedHost);

    bool getIsSendNode() const;
    void setIsSendNode(bool value);

private:
    int id_;
    bool hasConnectedHost_;
    MainScene * m_scene;

    bool isPathPart = false;

    RoutingTable *current_routing_table;
    RoutingTable *future_routing_table;

    Global* global = Global::GetInstance();

    QList<NetworkLine*> connectedLines {};
    QMap<int, ConnectedLineDetails*> nodeToLine {};
    QQueue<NetworkPackage*> *shouldSend {};
    QList<NetworkPackage*> packageToProcess {};
    bool sendNextPackage = false; //for logic and virtual channels
    bool isSendNode = false;

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

signals:
    void isClicked(NetworkNode* node);
};

#endif // NETWORKNODE_H
