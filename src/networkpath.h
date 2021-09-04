#ifndef NETWORKPATH_H
#define NETWORKPATH_H

#include <QList>

class NetworkNode;
class NetworkLine;

struct PathElement{
    NetworkNode* node;
    NetworkLine* line;
};

class NetworkPath
{
public:
    NetworkPath(QList<PathElement*> path);
    QList<PathElement*> getPath() const;
    QString getTextReprez() const;

    void addToPath(PathElement* pathElemet);

private:
    QList<PathElement*> path;

    QString textReprezentaiton;

    void generateTextReprezentation();
};

#endif // NETWORKPATH_H
