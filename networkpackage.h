#ifndef NETWORKPACKAGE_H
#define NETWORKPACKAGE_H

#include <QGraphicsObject>

enum class PackageType{Datagram, TCP};

class NetworkPackage : public QGraphicsObject
{
    Q_OBJECT
public:
    NetworkPackage(QString packageName,int id, PackageType type,  QGraphicsObject *parent = nullptr);

    QString getPackageName() const;
    void setPackageName(const QString &value);

    void setPosition(QPointF pos);

    int getId() const;
    void setId(int value);

    int getHeader_size() const;
    void setHeader_size(int value);

    int getData_size() const;
    void setData_size(int value);

private:
    PackageType type;
    QString packageName;
    int id;
    int header_size;
    int data_size;


    // QGraphicsItem interface
public:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

#endif // NETWORKPACKAGE_H
