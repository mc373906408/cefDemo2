#include "include/Tools/Tools.h"

#include <QGuiApplication>
#include <QScreen>

Tools::Tools()
{

}

Tools::~Tools()
{

}

int Tools::getMouseDesktopInt(const QPoint &mouse)
{
    auto lscreens = QGuiApplication::screens();
    int i =0;

    foreach(auto scr ,lscreens){
        int x1=scr->geometry().x();
        int y1=scr->geometry().y();
        int x2=scr->geometry().width()+x1;
        int y2=scr->geometry().height()+y1;
        if(mouse.x()>=x1&&mouse.x()<=x2)
        {
            if(mouse.y()>=y1&&mouse.y()<=y2)
            {
                return i;
            }
        }
        i++;
    }
    return 0;
}

QRect Tools::getDesktopRect()
{
    QScreen * screen =QGuiApplication::primaryScreen();
    return screen->geometry();
}
QRect Tools::getAvailableRect()
{
    QScreen * screen =QGuiApplication::primaryScreen();
    return  screen->availableGeometry();
}
QRect Tools::getNowDesktopRect(const QPoint &mouse)
{
    int t=getMouseDesktopInt(mouse);
    auto lscreens = QGuiApplication::screens();
    return lscreens[t]->geometry();

}
QRect Tools::getNowAvailableRect(const QPoint &mouse)
{
    int t=getMouseDesktopInt(mouse);
    auto lscreens = QGuiApplication::screens();
    return lscreens[t]->availableGeometry();
}

double Tools::getZoomFactor(const int &id)
{
    /* 可能存在的优化：使用静态变量存储值，并监听系统改变界面信号，重置该值 */
    QScreen* screen = QGuiApplication::screens()[id];
    int dpi = static_cast<int>(screen->logicalDotsPerInch());
    int devicePixel = static_cast<int>(screen->devicePixelRatio());

    if (devicePixel == 1) {
        if(dpi<=96){
            return 1.0;
        }else if(dpi<=120){
            return 1.25;
        }else if(dpi<=144){
            return 1.5;
        }else if(dpi<=168){
            return 1.75;
        }else if(dpi<=192){
            return 2.0;
        }else if(dpi>192){
            return 2.0;
        }
    }

    if (devicePixel == 2) {
        if(dpi<=72){
            return 1.5;
        }else if (dpi<=84){
            return 1.75;
        }else if(dpi<=96){
            return 2.0;
        }else if(dpi>96){
            return 2.0;
        }
    }
    return 1.0;
}

double Tools::getNowZoomFactor(const QPoint &mouse)
{
    int t=getMouseDesktopInt(mouse);
    return getZoomFactor(t);
}

