#ifndef NETWORKPACKAGE_H
#define NETWORKPACKAGE_H

#include "networkpath.h"

#include <QGraphicsObject>

enum class PackageType{Info, Service};
enum class PackageStatus{Sending, InBuff, OutBuff , Sent, Killed,Init,Processing};
enum class PackageServiceType{EnqConn, ConfConn, PackageConfirmation, EnqDis, ConfDis};

class NetworkPackage : public QGraphicsObject
{
    Q_OBJECT
public:
    NetworkPackage(QString packageName,int id, int from_node, int dest_node, PackageType type,  QGraphicsObject *parent = nullptr);

    QString getPackageName() const;
    void setPackageName(const QString &value);
    void setPosition(NetworkNode* node1, NetworkNode *node2);

    int getId() const;
    void setId(int value);

    int getHeader_size() const;
    void setHeader_size(int value);

    int getData_size() const;
    void setData_size(int value);

    void addToPackageLog(QString info);

    void addPathElement(PathElement* pathElement);

    int getPackageSize();
    void incrementSendingTime();

private:
    PackageType type;
    PackageStatus packageStatus = PackageStatus::Init;
    PackageServiceType packageServiceType; //only for service package
    QString packageName;
    QString packageLog;
    NetworkPath *packagePath = new NetworkPath({});
    int id;
    int sending_time = 0; //show how long package was transferred
    int header_size = 0;
    int data_size = 0;
    int dest_node = 0;
    int from_node = 0;
    int TTL = 64;

    int bytesToSentThrougLine = 0; //to Other node


    // QGraphicsItem interface
public:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QString getPackageLog() const;
    PackageType getType() const;
    NetworkPath *getPackagePath() const;
    void setPackageStatus(const PackageStatus &value);
    PackageServiceType getPackageServiceType() const;
    void setPackageServiceType(const PackageServiceType &value);
    PackageStatus getPackageStatus() const;
    int getTTL() const;
    void setTTL(int value);
    int getFrom_node() const;
    void setFrom_node(int value);
    int getDest_node() const;
    void setDest_node(int value);
    int getBytesToSentThrougLine() const;
    void setBytesToSentThrougLine(int value);
    int getSending_time() const;
};

#endif // NETWORKPACKAGE_H
