#ifndef MAINVIEW_H
#define MAINVIEW_H

#include "global.h"

#include <QGraphicsView>
#include <QObject>

class MainView : public QGraphicsView
{
    Q_OBJECT
public:
    MainView(QWidget *parent = nullptr);
    MainView(QGraphicsScene *scene, QWidget *parent = nullptr);
    void toggleSelectionMode();
    void toggleMoveMode();
protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
private:
    int originX__;
    int originY__;
    bool is_move__ = false;
    bool is_selection__ = true;
    void startSetup();
    Global *global = Global::GetInstance();
public slots:
    void zoomIn() {scale(1.2,1.2);}
    void zoomOut() {scale(1/1.2, 1/1.2);}
signals:
    void nodeShouldBeCreated(const QPointF& point);
};

#endif // MAINVIEW_H
