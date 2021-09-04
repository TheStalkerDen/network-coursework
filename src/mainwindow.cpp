#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QJsonDocument>
#include <QMessageBox>
#include <qjsonobject.h>
#include <routingtable.h>
#include <mainscene.h>
#include <QStringListModel>
#include "networknode.h"
#include "networkline.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    global = Global::GetInstance();
    global->m_main_window = this;
    scene = new MainScene();
    ui->graphicsView->setScene(scene);
    ui->autoWeightRadioButton->setChecked(true);
    connect(ui->graphicsView,&MainView::nodeShouldBeCreated, scene, &MainScene::addNode);
    connect(scene,&MainScene::showNode, this, &MainWindow::showNodeInfo);
    ui->ByWeightsRadioButton->setChecked(true);

    ui->nextStepButton->setDisabled(true);
    ui->endConfigurationButton->setDisabled(true);
    ui->showPathsGroupBox->setDisabled(true);
    ui->enableEditModeButton->setDisabled(true);
}

Ui::MainWindow *MainWindow::getUI() const
{
    return ui;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showNodeInfo(NetworkNode *node)
{
    currentShowNode = node;
    ui->nodeIdLabel->setText(QString::number(node->getId()));
    ui->isHostCheckBox->setChecked(node->getHostStatus());
}

void MainWindow::showLineDetails(NetworkLine *line)
{
    if(currentSelectedLine && currentSelectedLine != line){
        currentSelectedLine->setIsSelected(false);
    }
    currentSelectedLine = line;
    ui->isHalfDuplex->setChecked(line->getIsHalfDuplex());
    int index = ui->lineWeightComboButton->findText(QString::number(line->getWeight()));
    ui->lineWeightComboButton->setCurrentIndex(index);
    ui->error_chanceLineEdit->setText(QString::number(currentSelectedLine->getError_possibility()));
}

void MainWindow::showNetworkDegree(double networkDegree)
{
    ui->networkDegreeValue->setText(QString::number(networkDegree));
}

void MainWindow::setAlgoCounter(int value)
{
    ui->stepCounterLabel->setText("Step: " + QString::number(value));
}

void MainWindow::setStationsLists()
{
    QList<NetworkNode*> stations = scene->getStationsList();
    std::sort(stations.begin(),stations.end(),[](NetworkNode* node1, NetworkNode* node2){
        if(node1->getId() < node2->getId()){
            return true;
        }
        return false;
    });

    QStringList stationsStringList;

    for(auto& node: stations){
        stationsStringList << QString::number(node->getId());
    }
    ui->station1ComboBox->clear();
    ui->station1ComboBox->addItems(stationsStringList);
    ui->station2ComboBox->clear();
    ui->station2ComboBox->addItems(stationsStringList);
}

void MainWindow::generatesPathsList(QList<NetworkPath *> paths)
{
    QStringListModel *model = new QStringListModel();
    QStringList pathsList;
    int counter = 1;
    for(auto& path: paths){
        pathsList << "Path " + QString::number(counter);
        counter++;
    }
    model->setStringList(pathsList);
    ui->pathsListView->setModel(model);
}

void MainWindow::on_actionZoom_triggered()
{
    ui->graphicsView->zoomIn();
}

void MainWindow::on_actionZoom_2_triggered()
{
    ui->graphicsView->zoomOut();
}

void MainWindow::on_actionSave_triggered()
{
    QString saveFilename = QFileDialog::getSaveFileName(this,
                                                        "Save network configuration",
                                                        "",
                                                        "Json (*.json);;All files (*)");
    if (saveFilename.isEmpty())
            return;
    else {
        QFile file(saveFilename);
        if(!file.open(QIODevice::WriteOnly)){
            QMessageBox::information(this,"Unable to open file", file.errorString());
            return;
        }
        QJsonObject jsonObj;
        scene->writeSceneJson(jsonObj);
        QJsonDocument saveDoc(jsonObj);
        file.write(saveDoc.toJson());
    }
}

void MainWindow::on_actionOpen_triggered()
{
    QString openFilename = QFileDialog::getOpenFileName(this,"Open network configuration",
                                                        "",
                                                        "Json (*.json);;All files (*)");
    if(openFilename.isEmpty())
        return;
    else {
        QFile file(openFilename);
        if(!file.open(QIODevice::ReadOnly)){
            QMessageBox::information(this,"Unable to open file", file.errorString());
            return;
        }
        QByteArray saveData = file.readAll();
        QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
        QJsonObject loadObj = loadDoc.object();
        scene->initializeFromJson(loadObj);
    }
}

void MainWindow::on_actionMove_triggered(bool checked)
{
    if(checked){
        ui->graphicsView->toggleMoveMode();
        ui->actionSelection->setChecked(false);
        ui->graphicsView->setCursor(Qt::OpenHandCursor);
    }
}

void MainWindow::on_actionSelection_triggered(bool checked)
{
    if(checked){
        ui->graphicsView->toggleSelectionMode();
        ui->actionMove->setChecked(false);
        ui->graphicsView->setCursor(Qt::ArrowCursor);
    }
}

void MainWindow::on_autoWeightRadioButton_toggled(bool checked)
{
    if(checked){
        scene->setWeightMode(true);
    }
}

void MainWindow::on_manualWeightRadioButton_toggled(bool checked)
{
    if(checked){
        scene->setWeightMode(false);
    }
}

void MainWindow::on_manualWeightComboBox_currentIndexChanged(int index)
{
    int weight = ui->manualWeightComboBox->itemText(index).toInt();
    scene->setManualWeight(weight);
}

void MainWindow::on_isHostCheckBox_stateChanged(int arg1)
{
    if(currentShowNode != nullptr){
        currentShowNode->setHostStatus(arg1);
    }
    scene->update();
}

void MainWindow::on_startRoutingAlgoButton_clicked()
{
    RoutingMetrics rout_metr = ui->ByNodesRadioButton->isChecked() ? RoutingMetrics::ByNodes : RoutingMetrics::ByWeights;
    scene->startRoutingAlgorithm(rout_metr);
    ui->startRoutingAlgoButton->setDisabled(true);
    ui->nextStepButton->setEnabled(true);
    ui->endConfigurationButton->setEnabled(true);
    ui->stepCounterLabel->setText("Step: 0");
    global->is_simulation = true;
    ui->graphEditorTab->setDisabled(true);
    ui->enableEditModeButton->setEnabled(true);
    if(currentSelectedLine){
        currentSelectedLine->setIsSelected(false);
    }
}

void MainWindow::on_nextStepButton_clicked()
{
    bool wasChanged = scene->algoStep();
    if(!wasChanged){
        ui->showPathsGroupBox->setEnabled(true);
        ui->routingTableConfGroupBox->setDisabled(true);
        ui->sendTab->setEnabled(true);
        setStationsLists();
        setNodesComboBoxes();
    }
}

void MainWindow::on_endConfigurationButton_clicked()
{
    scene->endRoutingAlgo();
    ui->showPathsGroupBox->setEnabled(true);
    ui->routingTableConfGroupBox->setDisabled(true);
    ui->sendTab->setEnabled(true);
    setStationsLists();
    setNodesComboBoxes();
}

void MainWindow::on_enableEditModeButton_clicked()
{
    ui->nextStepButton->setDisabled(true);
    ui->endConfigurationButton->setDisabled(true);
    ui->showPathsGroupBox->setDisabled(true);
    ui->startRoutingAlgoButton->setEnabled(true);
    ui->stepCounterLabel->setText("Step:");
    ui->graphEditorTab->setEnabled(true);
    ui->routingTableConfGroupBox->setEnabled(true);

    ui->pathsListView->setModel({});
    ui->textPathReprez->setText("");

    scene->enableEditMode();
}

void MainWindow::on_getShortestPathsButton_clicked()
{
    int node1 = ui->node1ComboBox->currentText().toInt();
    int node2 = ui->node2ComboBox->currentText().toInt();
    if(node1 != node2 && scene->existsNode(node1) && scene->existsNode(node2)){
        qDebug() << "Start get shortest paths between " << node1 << " " << node2;
        scene->getShortestPaths(node1,node2);
    }
}

void MainWindow::on_pathsListView_clicked(const QModelIndex &index)
{
    NetworkPath* path = scene->getPaths().at(index.row());
    ui->textPathReprez->setText(path->getTextReprez());
    scene->drawPath(index.row());
}


void MainWindow::on_isHalfDuplex_stateChanged(int arg1)
{
    if(currentSelectedLine){
        currentSelectedLine->setIsHalfDuplex(arg1);
    }
}

void MainWindow::on_lineWeightComboButton_currentIndexChanged(const QString &arg1)
{
    if(currentSelectedLine){
        int weight = ui->lineWeightComboButton->currentText().toInt();
        currentSelectedLine->setWeight(weight);
    }
}

void MainWindow::on_startSendingButton_clicked()
{
    int node1 = ui->station1ComboBox->currentText().toInt();
    int node2 = ui->station2ComboBox->currentText().toInt();
    if(node1 != node2){
        int packageSize = ui->packageSizeLineEdit->text().toInt();
        int headerSize = ui->headerSizeLineEdit->text().toInt();
        int messageSize = ui->messageSizeLineEdit->text().toInt();
        SendingType type;
        if(ui->datagramRadioButton->isChecked()){
            type = SendingType::Datagram;
        }else if (ui->logConnectionRadioButton->isChecked()){
            type = SendingType::LogicalConnection;
        }else if (ui->virtChannelRadioButton->isChecked()){
            type = SendingType::VirualChannel;
        } else {
            type = SendingType::Datagram;
        }

        bool isRealTime = ui->realtimeRadioButton->isChecked();
        scene->startSimulation(node1,node2,messageSize,packageSize,headerSize,type,isRealTime);
    }
}

void MainWindow::setNodesComboBoxes()
{
    QList<NetworkNode*> stations = scene->getStationsList();
    std::sort(stations.begin(),stations.end(),[](NetworkNode* node1, NetworkNode* node2){
        if(node1->getId() < node2->getId()){
            return true;
        }
        return false;
    });

    QStringList stationsStringList;

    for(auto& node: stations){
        stationsStringList << QString::number(node->getId());
    }

    ui->node1ComboBox->clear();
    ui->node1ComboBox->addItems(stationsStringList);
    ui->node2ComboBox->clear();
    ui->node2ComboBox->addItems(stationsStringList);
}

void MainWindow::on_error_chanceLineEdit_returnPressed()
{
    if(currentSelectedLine){
        currentSelectedLine->setError_possibility(ui->error_chanceLineEdit->text().toInt());
    }
}
