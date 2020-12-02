#include "networkpath.h"
#include "networknode.h"

NetworkPath::NetworkPath(QList<PathElement*> path)
{
    this->path = path;
    generateTextReprezentation();
}

QList<PathElement *> NetworkPath::getPath() const
{
    return path;
}

QString NetworkPath::getTextReprez() const
{
    return textReprezentaiton;
}

void NetworkPath::generateTextReprezentation()
{
    textReprezentaiton = "";
    for(auto& netwElem: path){
        textReprezentaiton += QString::number(netwElem->node->getId());
        textReprezentaiton += " -> ";
    }
}
