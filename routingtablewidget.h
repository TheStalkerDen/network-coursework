#ifndef ROUTINGTABLEWIDGET_H
#define ROUTINGTABLEWIDGET_H

#include "routingtable.h"

#include <QWidget>

namespace Ui {
class routingtablewidget;
}

class RoutingTableWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RoutingTableWidget(RoutingTable* table, QWidget *parent = nullptr);
    ~RoutingTableWidget();

private:
    Ui::routingtablewidget *ui;
    RoutingTable* table;
};

#endif // ROUTINGTABLEWIDGET_H
