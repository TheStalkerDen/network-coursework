#include "simulationdatawidget.h"
#include "ui_simulationdatawidget.h"
#include "networkpackage.h"
#include "mainscene.h"

#include <QStringListModel>
#include <QDebug>

SimulationDataWidget::SimulationDataWidget(QWidget *parent) :
    QWidget(parent,Qt::Window),
    ui(new Ui::SimulationDataWidget)
{
    ui->setupUi(this);
    processAndDisplaySimData();
}



SimulationDataWidget::~SimulationDataWidget()
{
    delete ui;
}

void SimulationDataWidget::processAndDisplaySimData()
{
    qDebug() << "starting showing of simulation data";
    SimulationData &simData = global->simData;
    ui->ticksCount->setText(QString::number(simData.tick_count));

    ui->packageCountLabel->setText(QString::number(simData.all_packages.count()));
    int service_packages = 0;
    int service_sent_bytes = 0;
    int info_packages = 0;
    int allSentBytes = 0;
    int infoBytesCount = 0;
    int headerBytesCount = 0;
    QStringList packagesStringList;
    for(auto& package: simData.all_packages){
        switch (package->getType()) {
            case PackageType::Service :
                service_packages++;
                service_sent_bytes += package->getData_size() + package->getHeader_size();
                break;
            case PackageType::Info:
                info_packages++;
                break;
      }
      allSentBytes += package->getData_size() + package->getHeader_size();
      headerBytesCount += package->getHeader_size();
      infoBytesCount += package->getData_size();
      packagesStringList += package->getPackageName();
    }
    ui->infoPackageCountLabel->setText(QString::number(info_packages));
    ui->servicePackageCountLabel->setText(QString::number(service_packages));
    ui->sentServiceBytesLabel->setText(QString::number(service_sent_bytes));
    ui->sentBytesLabel->setText(QString::number(allSentBytes));
    ui->sentHeaderBytes->setText(QString::number(headerBytesCount));
    ui->sentInfoBytesLabel->setText(QString::number(infoBytesCount));

    QStringListModel *packagesStringListModel = new QStringListModel();
    packagesStringListModel->setStringList(packagesStringList);
    ui->packagesList->setModel(packagesStringListModel);
}

void SimulationDataWidget::showPackageData(NetworkPackage *package)
{
    qDebug() << "start show package data";
    global->main_scene->drawPath(package->getPackagePath());
    ui->packagIdLabel->setText(QString::number(package->getId()));
    ui->packageSize->setText(QString::number(package->getData_size() + package->getHeader_size()));
    ui->packageLog->setText(package->getPackageLog());
}

void SimulationDataWidget::on_packagesList_clicked(const QModelIndex &index)
{
    qDebug() << "was package clicked!";
    NetworkPackage* package = global->simData.all_packages.at(index.row());
    showPackageData(package);
}
