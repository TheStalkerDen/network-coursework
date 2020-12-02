#ifndef NETWORKNODE_H
#define NETWORKNODE_H

#include <QContextMenuEvent>
#include <QGraphicsObject>
#include <QList>

#include "global.h"

class NetworkLine;
class MainScene;
enum class RoutingMetrics;
class RoutingTable;

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
    bool routingAlgorithmStep();
    RoutingTable* getRoutingTable() const;
    void switchTables();

    void setPathPart();
    void unsetPathPart();

private:
    int id_;
    bool hasConnectedHost_;
    MainScene * m_scene;

    bool isPathPart = false;

    RoutingTable *current_routing_table;
    RoutingTable *future_routing_table;

    Global* global = Global::GetInstance();

    QList<NetworkLine*> connectedLines {};

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

signals:
    void isClicked(NetworkNode* node);
};

#endif // NETWORKNODE_H
