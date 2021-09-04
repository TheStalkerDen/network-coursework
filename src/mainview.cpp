#include "mainscene.h"
#include "mainview.h"

#include <QIcon>
#include <QMenu>
#include <QDebug>
#include <QContextMenuEvent>

MainView::MainView(QWidget *parent)
    :QGraphicsView(parent)
{
    startSetup();
}

MainView::MainView(QGraphicsScene *scene, QWidget *parent)
    :QGraphicsView(scene,parent)
{
    startSetup();
}

void MainView::toggleSelectionMode()
{
    is_selection__ = true;
    is_move__ = false;
}

void MainView::toggleMoveMode()
{
    is_move__ = true;
    is_selection__ = false;
}

void MainView::contextMenuEvent(QContextMenuEvent *event)
{
    if(itemAt(event->pos()) == nullptr && !global->is_simulation){
        QMenu menu;

        QAction* action_firstAction = menu.addAction(
                    QIcon(),
                    QString("Add node")
                    );
        menu.addSeparator();
        QAction* selected_action = menu.exec(event->globalPos());
        if(selected_action) {
            if(selected_action == action_firstAction)
            {
                const QPointF scenePos = mapToScene(event->pos());
                emit nodeShouldBeCreated(scenePos);
            }
        }
    } else if(itemAt(event->pos()) == nullptr &&  global->is_simulation) {
        QMenu menu;

        QAction *undrawPath = menu.addAction(
                    QIcon(),
                    QString("clear path")
                    );
        QAction* selected_action = menu.exec(event->globalPos());
        if(selected_action){
            if(selected_action == undrawPath)
            {
                qobject_cast<MainScene*>(scene())->undrawPath();
            }
        }
    } else {
        QGraphicsView::contextMenuEvent(event);
    }
}

void MainView::mousePressEvent(QMouseEvent *event)
{
    if(is_move__ && event->button() == Qt::LeftButton){
        originX__ = event->x();
        originY__ = event->y();
    }else {
        QGraphicsView::mousePressEvent(event);
    }
}

void MainView::mouseMoveEvent(QMouseEvent *event)
{
    if(is_move__ && event->buttons() & Qt::LeftButton){
        QPointF oldp = mapToScene(originX__,originY__);
        QPointF newp = mapToScene(event->pos());
        QPointF translation = newp - oldp;

        translate(translation.x(), translation.y());

        originX__ = event->x();
        originY__ = event->y();
    } else {
        QGraphicsView::mouseMoveEvent(event);
    }
}

void MainView::startSetup()
{
    setTransformationAnchor(QGraphicsView::NoAnchor);
    setRenderHint(QPainter::HighQualityAntialiasing);
}
