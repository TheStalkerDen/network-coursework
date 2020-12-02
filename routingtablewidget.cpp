#include "routingtablewidget.h"
#include "ui_routingtablewidget.h"

RoutingTableWidget::RoutingTableWidget(RoutingTable* table,QWidget *parent) :
    QWidget(parent,Qt::Window),
    ui(new Ui::routingtablewidget)
{
    ui->setupUi(this);
    this->table = table;
    ui->routingTableView->setModel(table);
}

RoutingTableWidget::~RoutingTableWidget()
{
    delete ui;
}
