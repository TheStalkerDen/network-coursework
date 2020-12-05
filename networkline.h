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

    int bytesPerTick();

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

    bool isPathPart = false;
    QPen defaultPen;

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
