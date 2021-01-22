#ifndef TOOLS_H
#define TOOLS_H

#include "global/CefClient_share_global.h"
#include <QObject>
#include <QRect>

class CEFCLIENT_SHARE_EXPORT Tools:public QObject
{
    Q_OBJECT
public:
    Tools();
    ~Tools();

    /*鼠标在主窗口时，判断屏幕序号*/
    int getMouseDesktopInt(const QPoint &mouse);
    /*实际主桌面RECT*/
    QRect getDesktopRect();
    /*实际当前桌面RECT*/
    QRect getNowDesktopRect(const QPoint &mouse);
    /*可用主桌面RECT，不含任务栏*/
    QRect getAvailableRect();
    /*可用当前桌面RECT，不含任务栏*/
    QRect getNowAvailableRect(const QPoint &mouse);
    /*主桌面DPI*/
    double getZoomFactor(const int &id=0);
    /*当前桌面DPI*/
    double getNowZoomFactor(const QPoint &mouse);

    int getDesktopWidth() {return getDesktopRect().width(); }
    int getDesktopHeight() { return getDesktopRect().height(); }
    int getDesktopX() {return getDesktopRect().x(); }
    int getDesktopY() { return getDesktopRect().y(); }

    int getNowDesktopWidth(const QPoint &mouse) {return getNowDesktopRect(mouse).width(); }
    int getNowDesktopHeight(const QPoint &mouse) { return getNowDesktopRect(mouse).height(); }
    int getNowDesktopX(const QPoint &mouse) {return getNowDesktopRect(mouse).x(); }
    int getNowDesktopY(const QPoint &mouse) { return getNowDesktopRect(mouse).y(); }

    int getAvailableWidth() {return getAvailableRect().width(); }
    int getAvailableHeight() { return getAvailableRect().height(); }
    int getAvailableX() {return getAvailableRect().x(); }
    int getAvailableY() { return getAvailableRect().y(); }

    int getNowAvailableWidth(const QPoint &mouse) {return getNowAvailableRect(mouse).width(); }
    int getNowAvailableHeight(const QPoint &mouse) { return getNowAvailableRect(mouse).height(); }
    int getNowAvailableX(const QPoint &mouse) {return getNowAvailableRect(mouse).x(); }
    int getNowAvailableY(const QPoint &mouse) { return getNowAvailableRect(mouse).y(); }


private:

};

#endif // TOOLS_H
