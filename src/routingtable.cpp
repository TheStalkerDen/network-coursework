#include "routingtable.h"
#include "mainscene.h"

RoutingTable::RoutingTable(int node_id, QList<RoutingEntity> init_list, RoutingMetrics metrics, QObject *parent)
    :QAbstractTableModel(parent), m_node_id(node_id)
{
    m_routing_metrics = metrics;
    for(auto& init_elem: init_list){
        if(global->main_scene->getIdToNodeMap()[init_elem.nodes[0]]->getHostStatus() == true){
            if(m_routing_metrics == RoutingMetrics::ByWeights){
                m_routing_table.insert(init_elem.nodes[0], init_elem);
            } else {
                m_routing_table.insert(init_elem.nodes[0], {init_elem.nodes, 0});
            }
        }
        if(m_routing_metrics == RoutingMetrics::ByWeights){
            m_weights[init_elem.nodes[0]] = init_elem.metrics;
        }
    }
    generateRows();
}

QList<int> RoutingTable::getNextNodes(int dest_node) const
{
    return m_routing_table[dest_node].nodes;
}

bool RoutingTable::updateTable(QList<QPair<int, RoutingTable *>> tables_from_connected_nodes)
{
    bool was_change = false;
    for(auto& pair: tables_from_connected_nodes){
        RoutingMapIterator itr(pair.second->getRoutingMap());
        while(itr.hasNext()){
            itr.next();
            if (m_routing_table.contains(itr.key())){
                int new_value = 0;
                if(m_routing_metrics == RoutingMetrics::ByWeights){
                    new_value = m_weights[pair.first] + itr.value().metrics;
                } else {
                    new_value = itr.value().metrics + 1;
                }
                int current_value = m_routing_table[itr.key()].metrics;
                RoutingEntity &current_entity = m_routing_table[itr.key()];
                if(new_value < current_value){
                    current_entity.metrics = new_value;
                    current_entity.nodes.clear();
                    current_entity.nodes.append(pair.first);
                    was_change = true;
                } else if (new_value == current_value && !current_entity.nodes.contains(pair.first)){
                    current_entity.nodes.append(pair.first);
                    was_change = true;
                }
            } else if (m_node_id != itr.key()){
                int new_value = 0;
                if(m_routing_metrics == RoutingMetrics::ByWeights){
                    new_value = m_weights[pair.first] + itr.value().metrics;
                } else {
                    new_value = itr.value().metrics + 1;
                }
                m_routing_table.insert(itr.key(), {{pair.first},new_value});
                was_change = true;
            }
        }
    }
    generateRows();
    return was_change;
}

void RoutingTable::setRoutingTable(const RoutingMap &table)
{
    m_routing_table.clear();
    RoutingMapIterator itr(table);
    while(itr.hasNext()){
        itr.next();
        m_routing_table[itr.key()].metrics = itr.value().metrics;
        m_routing_table[itr.key()].nodes = itr.value().nodes;
    }
    generateRows();
}

const RoutingMap &RoutingTable::getRoutingMap() const
{
    return m_routing_table;
}

void RoutingTable::generateRows()
{
    m_rows.clear();
    RoutingMapIterator itr(m_routing_table);
    while(itr.hasNext()){
        itr.next();
        const RoutingEntity& entity = itr.value();
        for(const auto& node_id: entity.nodes){
            m_rows.append({itr.key(), node_id, itr.value().metrics});
        }
    }
    setData(QModelIndex(),0,0);
}

int RoutingTable::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_rows.size();
}

int RoutingTable::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

QVariant RoutingTable::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(index.row() >= m_rows.size() || index.row() < 0)
        return QVariant();

    if(role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0:
                return m_rows[index.row()].dest_node;
            case 1:
                return m_rows[index.row()].next_node;
            case 2:
                return m_rows[index.row()].metric;
            default:
                break;
        }
    }
    return QVariant();
}

QVariant RoutingTable::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();

    if(orientation == Qt::Horizontal) {
        switch (section) {
            case 0:
                return "Dest Node";
            case 1:
                return "Next Node";
            case 2:
                return "Metric";
        }
    }
    return QVariant();
}

bool RoutingTable::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    beginInsertRows(QModelIndex(),row,row+count-1);
    endInsertRows();
    return true;
}


bool RoutingTable::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    beginRemoveRows(QModelIndex(),row,row+count-1);
    endRemoveRows();
    return true;
}



bool RoutingTable::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(value);
    Q_UNUSED(role);
    if(m_effective_row_count < rowCount()){
        insertRows(0,rowCount() - m_effective_row_count);
        m_effective_row_count= rowCount();
    } else if (m_effective_row_count > rowCount()){
        removeRows(0,m_effective_row_count - rowCount());
        m_effective_row_count = rowCount();
    }
    emit dataChanged(index,index,{Qt::DisplayRole,Qt::EditRole});
    return true;
}
