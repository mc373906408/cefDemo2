#include "MouseKeyHook.h"


bool MouseKeyHook::mouseMoveEvent(QHookMouseEvent *event)
{
    if(m_cefMoveClient){
        QCefView::getInstance()->ismove=true;
        m_cefMoveClient->root->setPosition(event->pos()-m_cefMoveClient->clickPos);
        /*移动他名下的子窗口*/
        foreach(QString childName,m_cefMoveClient->children){
            CefClientStruct* client=QCefView::getInstance()->getCefClientStruct(childName);
            if(client){
                client->root->setPosition(event->pos()-m_cefMoveClient->clickPos+QPoint(client->parent_left,client->parent_top));
            }
        }
    }else if(m_cefResizeClient){
        int n_width,n_height;
        if(m_width==0){
            n_width=m_cefResizeClient->root->width();
        }else{
            n_width=m_width+(event->pos().x()-m_cefResizeClient->clickPos.x());
            if(n_width<m_cefResizeClient->beginWidth){
                n_width=m_cefResizeClient->beginWidth;
            }
        }
        if(m_height==0){
            n_height=m_cefResizeClient->root->height();
        }else{
            n_height=m_height+(event->pos().y()-m_cefResizeClient->clickPos.y());
            if(n_height<m_cefResizeClient->beginHeight){
                n_height=m_cefResizeClient->beginHeight;
            }
        }
        m_cefResizeClient->root->resize(n_width,n_height);

    }
    return true;
}

bool MouseKeyHook::mousePressEvent(QHookMouseEvent *event)
{
    return true;
}

bool MouseKeyHook::mouseReleaseEvent(QHookMouseEvent *event)
{
    if(m_cefMoveClient){
        QCefView::getInstance()->ismove=false;
        m_cefMoveClient=nullptr;
    }
    if(m_cefResizeClient){
        m_cefResizeClient=nullptr;
    }
    return true;
}

bool MouseKeyHook::keyPressEvent(QHookKeyEvent *event)
{
    return true;
}

bool MouseKeyHook::keyReleaseEvent(QHookKeyEvent *event)
{
    return true;
}

void MouseKeyHook::moveWindow(CefClientStruct *cefMoveClient)
{
    cefMoveClient->clickPos=QCursor::pos()-cefMoveClient->root->position();
    m_cefMoveClient=cefMoveClient;
}

void MouseKeyHook::resizeWindow(CefClientStruct *cefResizeClient,int width,int height)
{
    cefResizeClient->clickPos=QCursor::pos();
    m_cefResizeClient=cefResizeClient;
    m_width=width;
    m_height=height;
}



MouseKeyHook::MouseKeyHook()
{
    m_hook=QHook::instance();
    m_hook->hookMouse();
    m_hook->hookKeyboard();
}
