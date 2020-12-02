#include "global.h"

Global* Global::global_ = nullptr;

Global *Global::GetInstance()
{
    if(global_ ==nullptr){
        global_ = new Global();
    }
    return global_;
}

Global::Global()
{


}
