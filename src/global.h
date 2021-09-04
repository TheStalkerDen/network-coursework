#ifndef GLOBAL_H
#define GLOBAL_H

#include <QWidget>

class MainWindow;
class NetworkPackage;
class MainScene;

enum class SendingType;

struct SimulationData{
    int tick_count = 0;
    int current_max_id = -1;
    int additional_service_package = 0;
    QList<NetworkPackage*> all_packages;
    QList<NetworkPackage*> packages_on_scene;
    int getPackageId();
};

class Global
{
public:
    void operator=(const Global&) = delete;
    static Global *GetInstance();
    MainWindow *m_main_window;
    MainScene *main_scene;
    bool is_simulation = false;
    SimulationData simData;
    SendingType sendingType;


protected:
    Global();

    static Global* global_;

};
#endif // GLOBAL_H
