#ifndef SIMULATIONDATAWIDGET_H
#define SIMULATIONDATAWIDGET_H

#include <QWidget>
#include "global.h"

namespace Ui {
class SimulationDataWidget;
}

class SimulationDataWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SimulationDataWidget(QWidget *parent = nullptr);
    ~SimulationDataWidget();

    void processAndDisplaySimData();
    void showPackageData(NetworkPackage* package);

private slots:
    void on_packagesList_clicked(const QModelIndex &index);

private:
    Ui::SimulationDataWidget *ui;
    Global *global = Global::GetInstance();
};

#endif // SIMULATIONDATAWIDGET_H
