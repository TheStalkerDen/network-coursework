#ifndef MAINSCENE_H
#define MAINSCENE_H

#include <QObject>
#include <QGraphicsScene>
#include <QList>
#include <QVector>
#include <global.h>
#include "networknode.h"
#include "networkpackage.h"
#include "networkpath.h"

class NetworkLine;
enum class RoutingMetrics;

namespace Ui { class MainWindow; }

enum class SendingType{Datagram,LogicalConnection,VirualChannel};

class MainScene : public QGraphicsScene
{
    Q_OBJECT
public:
    MainScene(QObject *parent = nullptr);

    void setUI();
    void setNodesSelectable(bool state);

    void writeSceneJson(QJsonObject &jsonObj);
    void initializeFromJson(QJsonObject &jsonObj);

    void setWeightMode(bool);
    void setManualWeight(int weight);
    void adjustNodeIds();
    void removeNode(NetworkNode* node);
    void removeLine(NetworkLine* line, NetworkNode* node=nullptr);
    void showNodeInfo(NetworkNode* node);

    bool algoStep();
    void endRoutingAlgo();
    bool existsNode(int node);


    void getShortestPaths(int fromNode, int destNode);
    QList<NetworkPath*> getPaths() const;
    void drawPath(int pathNumber);
    void drawPath(NetworkPath* networkPath);
    void undrawPath();

    void startSimulation(int fromNode, int toNode, int messageSize, int packageSize, int headerSize, SendingType send_type, bool isRealtime);

    QList<NetworkNode*> getStationsList();

    void calculateNetworkDegree();

    void enableEditMode();

    bool isSelect2Node = false;  //should be true when was selected first node for connecting
    QMap<int, NetworkNode *> getIdToNodeMap() const;

private:
    int max_id = 0;

    double networkDegree = 0;

    int algo_steps_counter = 0;
    QList<NetworkPath*> networkPaths;

    NetworkNode *selectedNode1 = nullptr;
    NetworkNode *selectedNode2 = nullptr;
    QList<NetworkNode*> networkNodes {};
    QMap<int,NetworkNode*> idToNodeMap {};
    QList<NetworkLine*> networkLines {};

    Global *global = Global::GetInstance();
    SimulationData simData;
    QTimer *timer;


    const QVector<int> weights = {1,3,5,6,8,10,11,15,18,20,22,26,27};

    bool isAutoWeight = true;
    int manual_weight = 1;

    void feelIdtoNodeMap();

    int getWeight();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
public slots:
    void addLine();
    void addNode(const QPointF& point);

    void generateNodeFromJson(QJsonObject &jsonObj);

    void commonNodeCreationPart(NetworkNode *node);
    void updateAll(const QList<QRectF> &region);

    void selectNode1(NetworkNode* node);
    void selectNode2(NetworkNode* node);

    void startRoutingAlgorithm(RoutingMetrics rout_metr);

signals:
    void selNodeChanged(int node);
    void showNode(NetworkNode* node);
    void showNetworkDegree(double networkDegree);
    void generatePathsList(QList<NetworkPath*> paths);

    void setCounterLabel(int value);
};

#endif // MAINSCENE_H
