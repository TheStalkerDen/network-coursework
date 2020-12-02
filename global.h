#ifndef GLOBAL_H
#define GLOBAL_H

#include <QWidget>

class MainWindow;

class Global
{
public:
    void operator=(const Global&) = delete;
    static Global *GetInstance();
    MainWindow *m_main_window;
    bool is_simulation = false;

protected:
    Global();

    static Global* global_;

};
#endif // GLOBAL_H
