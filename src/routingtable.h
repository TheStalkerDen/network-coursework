#ifndef ROUTINGTABLE_H
#define ROUTINGTABLE_H

#include "global.h"

#include <QAbstractTableModel>
#include <QList>
#include <QMap>

struct RoutingEntity{
    QList<int> nodes;
    int metrics;
};

struct TableRow{
    int dest_node;
    int next_node;
    int metric;
};

using RoutingMap = QMap<int,RoutingEntity>;
using RoutingMapIterator = QMapIterator<int,RoutingEntity>;

enum class RoutingMetrics{ByNodes, ByWeights};

class RoutingTable : public QAbstractTableModel
{
public:
    RoutingTable(int node_id, QList<RoutingEntity> init_list, RoutingMetrics metrics,QObject *parent = nullptr);
    QList<int> getNextNodes(int dest_node) const;
    bool updateTable(QList<QPair<int,RoutingTable*>> tables_from_connected_nodes);
    void setRoutingTable(const RoutingMap& table);
    const RoutingMap& getRoutingMap() const;
private:
    RoutingMap m_routing_table;
    RoutingMetrics m_routing_metrics;
    QMap<int,int> m_weights;
    QList<TableRow> m_rows;

    int m_node_id;

    int m_effective_row_count;

    void generateRows();
    Global *global = Global::GetInstance();

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
};

#endif // ROUTINGTABLE_H
