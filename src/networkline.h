#ifndef NETWORKLINE_H
#define NETWORKLINE_H

#include <QGraphicsObject>
#include <QGraphicsScene>
#include "global.h"

class MainScene;
class NetworkNode;

class NetworkLine : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit NetworkLine(NetworkNode *node1, NetworkNode *node2, int weight, QGraphicsObject *parent = nullptr);
    void writeJson(QJsonObject &jsonObj) const;
    void detachFromOtherNode(NetworkNode* node);
    void detachFromNodes();
    NetworkNode* returnOtherNode(NetworkNode* node);
    void update();
    int getWeight() const {return weight;}
    void setWeight(int weight);
    void setPathPart();
    void unsetPathPart();

    bool getIsHalfDuplex() const;
    void setIsHalfDuplex(bool value);

    int bytesPerTick() const;

    int getError_possibility() const;
    void setError_possibility(int value);

    void setIsSelected(bool value);

    void lineTick();
    void reloadLine(int ticks);

    int getIs_reloading() const;

    int getIs_used() const;
    void setIs_used(int value);

private:
    QRectF boundingRect() const override;
    QPointF calculateMiddlePoint();

    MainScene *m_scene;
    int weight;
    bool isHalfDuplex = false;
    NetworkNode *node1;
    NetworkNode *node2;
    QGraphicsLineItem *line;
    Global *global = Global::GetInstance();
    int error_possibility = 0;
    int is_used = -1; //for half duplex , actual value is id of node that uses line
    int reloading_time = 0; //for half duplex
    int is_reloading = false;

    bool isPathPart = false;
    QPen defaultPen;
    bool isSelected = false;


signals:
    void showLineDetails(NetworkLine *line);

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    // QGraphicsItem interface
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
};

#endif // NETWORKLINE_H
