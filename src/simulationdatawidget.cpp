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

    ui->packageCountLabel->setText(QString::number(simData.all_packages.count() + global->simData.additional_service_package));
    int service_packages = 0;
    int service_sent_bytes = 0;
    int info_packages = 0;
    int allSentBytes = 0;
    int infoBytesCount = 0;
    int headerBytesCount = 0;
    int killed_packages = 0;
    int lostinfobytes = 0;
    QStringList packagesStringList;

    std::sort(simData.all_packages.begin(), simData.all_packages.end(), [](const NetworkPackage* p1, const NetworkPackage* p2){
        return p1->getId() < p2->getId();
    });

    for(auto& package: simData.all_packages){
        switch (package->getType()) {
            case PackageType::Service :
                service_packages++;
                if(package->getPackageStatus() != PackageStatus::Killed){
                    service_sent_bytes += package->getData_size() + package->getHeader_size();
                }
                break;
            case PackageType::Info:
                info_packages++;
                if(package->getPackageStatus() != PackageStatus::Killed){

                }
                break;
      }
      if(package->getPackageStatus() == PackageStatus::Killed){
          killed_packages++;
      }
      allSentBytes += package->getData_size() + package->getHeader_size();
      headerBytesCount += package->getHeader_size();
      if(package->getPackageStatus() != PackageStatus::Killed){
          infoBytesCount += package->getData_size();
      } else {
        lostinfobytes += package->getData_size();
      }
      packagesStringList += package->getPackageName();
    }

    service_packages += global->simData.additional_service_package;
    service_sent_bytes += global->simData.additional_service_package * 8;


    ui->infoPackageCountLabel->setText(QString::number(info_packages));
    ui->servicePackageCountLabel->setText(QString::number(service_packages));
    ui->sentServiceBytesLabel->setText(QString::number(service_sent_bytes));
    ui->sentBytesLabel->setText(QString::number(allSentBytes));
    ui->sentHeaderBytes->setText(QString::number(headerBytesCount));
    ui->sentInfoBytesLabel->setText(QString::number(infoBytesCount));
    ui->lostPackagesCountLabel->setText(QString::number(killed_packages));
    ui->lostBytesLabel->setText(QString::number(lostinfobytes));

    QStringListModel *packagesStringListModel = new QStringListModel();
    packagesStringListModel->setStringList(packagesStringList);
    ui->packagesList->setModel(packagesStringListModel);
}

void SimulationDataWidget::showPackageData(NetworkPackage *package)
{
    qDebug() << "start show package data";
    global->main_scene->drawPath(package->getPackagePath());
    ui->packageIdLabel->setText(QString::number(package->getId()));
    ui->packageSizeLabel->setText(QString::number(package->getData_size() + package->getHeader_size()));
    ui->packageLog->setText(package->getPackageLog());
    ui->packageNameLabel->setText(package->getPackageName());
    ui->dataSizeLabel->setText(QString::number(package->getData_size()));
    ui->headerSizeLabel->setText(QString::number(package->getHeader_size()));
    QString packageType;
    if(package->getType() == PackageType::Info){
        packageType = "Info";
    } else {
        packageType = "Service";
    }
    ui->packageTypeLabel->setText(packageType);
    ui->sendingTimeLabel->setText(QString::number(package->getSending_time()));
}

void SimulationDataWidget::on_packagesList_clicked(const QModelIndex &index)
{
    qDebug() << "was package clicked!";
    NetworkPackage* package = global->simData.all_packages.at(index.row());
    showPackageData(package);
}
