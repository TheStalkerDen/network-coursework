#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QList>
#include <QMainWindow>
#include "global.h"
#include "networkpath.h"

class NetworkLine;
class NetworkNode;
class MainScene;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    Ui::MainWindow* getUI() const;
    ~MainWindow();

public slots:
    void showNodeInfo(NetworkNode* node);
    void showLineDetails(NetworkLine *line);
    void showNetworkDegree(double networkDegree);

    void setAlgoCounter(int value);

    void setStationsLists();

    void generatesPathsList(QList<NetworkPath*> paths);

private slots:

    void on_actionZoom_triggered();

    void on_actionZoom_2_triggered();

    void on_actionSave_triggered();

    void on_actionOpen_triggered();

    void on_actionMove_triggered(bool checked);

    void on_actionSelection_triggered(bool checked);

    void on_autoWeightRadioButton_toggled(bool checked);

    void on_manualWeightRadioButton_toggled(bool checked);

    void on_manualWeightComboBox_currentIndexChanged(int index);

    void on_isHostCheckBox_stateChanged(int arg1);

    void on_startRoutingAlgoButton_clicked();

    void on_nextStepButton_clicked();

    void on_endConfigurationButton_clicked();

    void on_enableEditModeButton_clicked();

    void on_getShortestPathsButton_clicked();

    void on_pathsListView_clicked(const QModelIndex &index);

    void on_isHalfDuplex_stateChanged(int arg1);

    void on_lineWeightComboButton_currentIndexChanged(const QString &arg1);

    void on_startSendingButton_clicked();

    void on_error_chanceLineEdit_returnPressed();

private:
    Ui::MainWindow *ui;
    MainScene *scene;
    NetworkNode *currentShowNode = nullptr;
    NetworkLine *currentSelectedLine = nullptr;
    Global* global;
    void setNodesComboBoxes();
};
#endif // MAINWINDOW_H
