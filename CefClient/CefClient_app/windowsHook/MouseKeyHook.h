#ifndef MOUSEKEYHOOK_H
#define MOUSEKEYHOOK_H

#include <QObject>
#include "qhook.h"
#include "browser/QCefView.h"

//enum HookEnum{
//    Mouse,
//    Keyboard
//};


class MouseKeyHook : public QHook
{
    Q_OBJECT
private:
    /*鼠标移动事件*/
    virtual bool mouseMoveEvent(QHookMouseEvent *event) override;
    /*鼠标点击事件*/
    virtual bool mousePressEvent(QHookMouseEvent *event) override;
    /*鼠标释放事件*/
    virtual bool mouseReleaseEvent(QHookMouseEvent *event) override;
    /*按键按压事件*/
    virtual bool keyPressEvent(QHookKeyEvent *event) override;
    /*按键释放事件*/
    virtual bool keyReleaseEvent(QHookKeyEvent *event) override;
public:
    static MouseKeyHook& getInstance(){
       static MouseKeyHook instance;
       return instance;
     }
    void moveWindow(CefClientStruct* cefMoveClient);
    void resizeWindow(CefClientStruct* cefResizeClient,int width,int height);

private:
    MouseKeyHook();
    MouseKeyHook(const MouseKeyHook &) = delete;
    MouseKeyHook &operator =(const MouseKeyHook &) = delete;

    QHook* m_hook=nullptr;
    CefClientStruct* m_cefMoveClient=nullptr;
    CefClientStruct* m_cefResizeClient=nullptr;

    int m_width=0;
    int m_height=0;


signals:

};

#endif // MOUSEKEYHOOK_H
