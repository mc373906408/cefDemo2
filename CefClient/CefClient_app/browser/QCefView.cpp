#include "QCefView.h"
#include <QUrl>
#include <QDebug>

#include <QJsonDocument>
#include <QCursor>

#include <QProcess>
#include <QDesktopServices>
#include <QCoreApplication>
#include <QTimer>


#include <wininet.h>

#include "CefClient_share.h"
#include "Tools/Tools.h"
#include "windowsHook/MouseKeyHook.h"


QCefView::QCefView()
{

}

QCefView::~QCefView()
{
    if(m_cefTimer){
        m_cefTimer->stop();
        delete m_cefTimer;
    }
}


void QCefView::init( QWindow *window, QCefContext *cef)
{
    m_window=window;
    m_cef=cef;

    /*cef的信息循环*/
    m_cefTimer = new QTimer(this);
    connect(m_cefTimer, SIGNAL(timeout()), this, SLOT(onCefTimer()));


}

void QCefView::onCefTimer()
{
    CefDoMessageLoopWork();
    if (m_cefClientsOpenList.isEmpty()) {
        return;
    }
    if (!m_cefClients.contains(m_cefClientsOpenList.at(0).first)) {
        return;
    }
    CefClientStruct *cefClient= &m_cefClients[m_cefClientsOpenList.at(0).first];
    QWindow* root = m_window->findChild<QWindow*>(cefClient->objectName);
    if(!root){
        return;
    }
    cefClient->root=root;
    root->setTitle(cefClient->objectName);

    connect(cefClient->root,&QWindow::widthChanged,[=](int arg){
        cefClient->width=arg;
        this->windowMetamorphosis(cefClient,Metamorphosis::Width);
    });
    connect(cefClient->root,&QWindow::heightChanged,[=](int arg){
        cefClient->height=arg;
        this->windowMetamorphosis(cefClient,Metamorphosis::Height);
    });

    /*如果只是打开窗口，不是打开网页*/
    if(!m_cefClientsOpenList.at(0).second){
        m_cefClientsOpenList.removeAt(0);
        return;
    }
    
    CefWindowHandle browserHandle=cefClient->client->browserWinId();
    if(browserHandle==(CefWindowHandle)-1){
        return;
    }
    m_cefClientsOpenList.removeAt(0);
    /*获取cookie*/
    CefRefPtr<CefCookieManager> cefCookieManager = CefCookieManager::GetGlobalManager(nullptr);
    if(cefCookieManager){
        cefCookieManager->VisitUrlCookies(cefClient->url.toStdString(),true,cefClient->client);
    }
    /*绑定父窗口*/
    QWindow *window=QWindow::fromWinId((WId)browserHandle);
    window->setParent(cefClient->root);
    window->resize(cefClient->width,cefClient->height);
    window->setX(0);
    window->setY(0);
    window->setVisible(true);

    /*绑定cefWindow*/
    cefClient->cefWnd=window;
    /*绑定唯一ID*/
    cefClient->identifier=cefClient->client->browser()->GetIdentifier();
    /*绑定信号槽*/
    QCefClient* cefClientPtr=cefClient->client.get();
    connect(cefClientPtr, SIGNAL(sgWebMsgReceived(QString)), this, SIGNAL(sgWebMsgReceived(QString)));

    /*窗口长宽自适应*/
    connect(cefClient->root,&QWindow::widthChanged,[=](int arg){
        cefClient->cefWnd->setWidth(arg);
    });
    connect(cefClient->root,&QWindow::heightChanged,[=](int arg){
        cefClient->cefWnd->setHeight(arg);
    });

    /*自动移动像素,修复界面错位bug*/
    cefClient->root->setWidth(cefClient->root->width()+1);
    cefClient->root->setWidth(cefClient->root->width()-1);
    cefClient->root->setHeight(cefClient->root->height()+1);
    cefClient->root->setHeight(cefClient->root->height()-1);
    if (cefClient->parent.isEmpty()) {
        return;
    }
    if(m_cefClients.contains(cefClient->parent)){
        CefClientStruct *client= &m_cefClients[cefClient->parent];
        cefClient->root->setPosition(client->root->position()+QPoint(cefClient->parent_left,cefClient->parent_top));
    }
}

void QCefView::windowMetamorphosis(CefClientStruct *cefClient, QCefView::Metamorphosis mode)
{
    if(!cefClient->parent.isEmpty()){
        if(m_cefClients.contains(cefClient->parent)){
            CefClientStruct *client= &m_cefClients[cefClient->parent];
            /*修改与父窗口的相对位置*/
            switch(mode){
            case Width:
                cefClient->parent_right=client->width-(cefClient->width+cefClient->parent_left);
                break;
            case Height:
                cefClient->parent_bottom=client->height-(cefClient->height+cefClient->parent_top);
                break;
            }
        }
    }

    foreach(QString childName,cefClient->children){
        if(m_cefClients.contains(childName)){
            CefClientStruct *client= &m_cefClients[childName];
            /*移动他名下的子窗口*/
            switch(mode){
            case X:
                client->root->setX(cefClient->root->x()+client->parent_left);
                break;
            case Y:
                client->root->setY(cefClient->root->y()+client->parent_top);
                break;
            case Width:
                client->root->setWidth(cefClient->root->width()-client->parent_left-client->parent_right);
                break;
            case Height:
                client->root->setHeight(cefClient->root->height()-client->parent_top-client->parent_bottom);
                break;
            }
        }
    }
}

QJsonObject QCefView::getWindowJson(const QString &objectName, const QString &fatherName, const QString &rect, const QString &align, const QString &valign, const QString &lostFocusHide, const QString &front, const bool &visible)
{
    QStringList m_rect=rect.split(',');
    double zoom=CefClientShare::getInstance().getTools()->getNowZoomFactor(QCursor::pos());
    int x=m_rect.at(0).toInt()*zoom;
    int y=m_rect.at(1).toInt()*zoom;
    int width=m_rect.at(2).toInt()*zoom-x;
    int height=m_rect.at(3).toInt()*zoom-y;

    QJsonObject json;
    json["objectName"]=objectName;
    json["x"]=x;
    json["y"]=y;
    json["width"]=width;
    json["height"]=height;
    json["visible"]=visible;
    json["flags"]=(int)m_window->flags();

    QRect p_rect;
    /*找下父窗口存不存在*/
    if(m_cefClients.contains(fatherName)){
        CefClientStruct *cefClient= &m_cefClients[fatherName];
        p_rect=QRect(cefClient->root->x(),cefClient->root->y(),cefClient->width,cefClient->height);
        json["x"]=x+p_rect.x();
        json["y"]=y+p_rect.y();
        if(lostFocusHide!="1"){
            json["width"]=width+cefClient->width-cefClient->beginWidth;
            json["height"]=height+cefClient->height-cefClient->beginHeight;
        }
    }else{
        p_rect=CefClientShare::getInstance().getTools()->getNowDesktopRect(QCursor::pos());
    }

    if(align=="Left"){
        json["x"]=p_rect.x();
    }else if(align=="Center"){
        json["x"]=(p_rect.width()-width)/2+p_rect.x();
    }else if(align=="Right"){
        json["x"]=p_rect.width()-width+p_rect.x();
    }
    if(valign=="Top"){
        json["y"]=p_rect.y();
    }else if(valign=="VCenter"){
        json["y"]=(p_rect.height()-height)/2+p_rect.y();
    }else if(valign=="Bottom"){
        json["y"]=p_rect.height()-height+p_rect.y();
    }

    if(front=="1"){
        json["modality"]=(int)Qt::WindowModal;
    }
    json["flags"]=(int)m_window->flags();
    json["visible"]=visible;

    return json;
}

CefClientStruct QCefView::getWindowCefClientStruct(const QString &objectName, const QString &fatherName, const QString &rect, const QString &align, const QString &valign, const QString &needHide, const QString &lostFocusHide, const QString &front, const bool &visible)
{
    CefClientStruct m_CefClientStruct;
    QJsonObject json=getWindowJson(objectName,fatherName,rect,align,valign,lostFocusHide,front,visible);
    m_CefClientStruct.json=json;
    m_CefClientStruct.objectName=objectName;
    m_CefClientStruct.beginWidth=json["width"].toInt();
    m_CefClientStruct.beginHeight=json["height"].toInt();

    /*找下父窗口存不存在*/
    QRect p_rect;
    if(m_cefClients.contains(fatherName)){
        CefClientStruct *cefClient= &m_cefClients[fatherName];
        p_rect=QRect(cefClient->root->x(),cefClient->root->y(),cefClient->width,cefClient->height);

        m_CefClientStruct.father=cefClient->root->focusObject();
        /*加到父窗口队列中，并排在第一位*/
        cefClient->children.push_front(objectName);
        m_CefClientStruct.parent=cefClient->objectName;

        if(needHide=="1"){
            /*将父窗口的子窗口页面中其他需要隐藏的隐藏*/
            for(auto cefClient=m_cefClients.begin();cefClient!=m_cefClients.constEnd();++cefClient){
                if(cefClient.value().parent==fatherName&&cefClient.value().needHide){
                    cefClient->root->setVisibility(QWindow::Hidden);
                }
            }
            m_CefClientStruct.needHide=true;
        }
    }else{
        p_rect=CefClientShare::getInstance().getTools()->getNowDesktopRect(QCursor::pos());
    }

    if(front=="1"){
        m_CefClientStruct.isFront=true;
    }

    m_CefClientStruct.width=json["width"].toInt();
    m_CefClientStruct.height=json["height"].toInt();
    m_CefClientStruct.parent_left=json["x"].toInt()-p_rect.x();
    m_CefClientStruct.parent_top=json["y"].toInt()-p_rect.y();
    m_CefClientStruct.parent_right=p_rect.width()-(m_CefClientStruct.parent_left+json["width"].toInt());
    m_CefClientStruct.parent_bottom=p_rect.height()-(m_CefClientStruct.parent_top+json["height"].toInt());

    if(lostFocusHide=="1"){
        m_CefClientStruct.lostFocusHide=true;
    }
    return m_CefClientStruct;
}



void QCefView::sendToWeb(QString objectName,QString msg)
{
    // 发送给H5信息，先发送给RENDERER，在RENDERER进程中转发给H5
    if(m_cefClients.contains(objectName)){
        CefClientStruct *cefClient= &m_cefClients[objectName];
        cefClient->client->browser()->GetMainFrame()->SendProcessMessage(PID_RENDERER,CefProcessMessage::Create(msg.toStdString().c_str()));
    }
}

void QCefView::setWindowStatus(QString objectName, QString status)
{
    if(m_cefClients.contains(objectName)){
        CefClientStruct *client= &m_cefClients[objectName];
        if(status=="Maximized"){
            client->win_x=client->root->x();
            client->win_y=client->root->y();
            client->win_width=client->root->width();
            client->win_height=client->root->height();
            QRect rect=CefClientShare::getInstance().getTools()->getNowAvailableRect(QCursor::pos());
            client->root->setX(rect.x());
            client->root->setY(rect.y());
            client->root->setWidth(rect.width());
            client->root->setHeight(rect.height());
            client->isMax=true;
            client->isFull=false;
        }else if(status=="Windowed"){
            client->root->setVisibility(QWindow::Windowed);
            if(client->isMax||client->isFull){
                client->isMax=false;
                client->isFull=false;
                client->root->setX(client->win_x);
                client->root->setY(client->win_y);
                client->root->setWidth(client->win_width);
                client->root->setHeight(client->win_height);
            }
        }else if(status=="Minimized"){
            m_window->setVisibility(QWindow::Minimized);
        }else if(status=="FullScreen"){
            client->win_x=client->root->x();
            client->win_y=client->root->y();
            client->win_width=client->root->width();
            client->win_height=client->root->height();
            QRect rect = CefClientShare::getInstance().getTools()->getNowAvailableRect(QCursor::pos());
            client->root->setX(rect.x());
            client->root->setY(rect.y());
            client->root->setWidth(rect.width());
            client->root->setHeight(rect.height());
            client->isFull=true;
        }
    }
}


void QCefView::setWindowFillScreen()
{
    for(auto cefClient=m_cefClients.begin();cefClient!=m_cefClients.constEnd();++cefClient){
        if(cefClient.value().isFull){
            cefClient.value().isFull=false;
            cefClient.value().root->setX(cefClient.value().win_x);
            cefClient.value().root->setY(cefClient.value().win_y);
            cefClient.value().root->setWidth(cefClient.value().win_width);
            cefClient.value().root->setHeight(cefClient.value().win_height);
        }
    }
}



void QCefView::doEveryLoop()
{
    m_cefTimer->start(10);
}

void QCefView::closeWindow(QString objectName)
{
    if(m_cefClients.contains(objectName)){
        CefClientStruct *client= &m_cefClients[objectName];
        if(client->lostFocusHide){
            client->root->hide();
        }else{
            client->root->close();
        }
    }
}

void QCefView::openUrl(const QString &objectName, const QString &fatherName, const QString &url, const QString &rect, const QString &align, const QString &valign, const QString &needHide, const QString &lostFocusHide, const QString &front, const bool &visible)
{
    if(!m_cef){
        return;
    }
    if(m_cefClients.contains(objectName)){
        load(objectName,url);
        return;
    }
    CefClientStruct m_CefClientStruct=getWindowCefClientStruct(objectName,fatherName,rect,align,valign,needHide,lostFocusHide,front,visible);

    CefRefPtr<QCefClient> client=m_cef->cefApp()->addBrowser(this,"app",false,url,m_CefClientStruct.width,m_CefClientStruct.height,0,0,nullptr);
    m_CefClientStruct.client=client;
    m_CefClientStruct.url=url;
    m_cefClients[objectName]=m_CefClientStruct;
    m_cefClientsOpenList.append(QPair<QString,bool>(objectName,true));
    emit sgOpenNewWindow(m_CefClientStruct.father,QString(QJsonDocument(m_CefClientStruct.json).toJson()));
}

void QCefView::openWindow(const QString &objectName, const QString &fatherName, const QString &rect, const QString &align, const QString &valign, const QString &needHide, const QString &lostFocusHide, const QString &front, const bool &visible)
{
    if(m_cefClients.contains(objectName)){
        return;
    }
    CefClientStruct m_CefClientStruct=getWindowCefClientStruct(objectName,fatherName,rect,align,valign,needHide,lostFocusHide,front,visible);
    m_cefClients[objectName]=m_CefClientStruct;
    m_cefClientsOpenList.append(QPair<QString,bool>(objectName,false));
    emit sgOpenNewWindow(m_CefClientStruct.father,QString(QJsonDocument(m_CefClientStruct.json).toJson()));
}


void QCefView::switchWindow(QString objectName, QString needHide)
{
    /*窗口是否已经打开*/
    if(m_cefClients.contains(objectName)){
        CefClientStruct *client= &m_cefClients[objectName];
        /*将父窗口的子窗口页面中其他需要隐藏的隐藏*/
        if(needHide=="1"){
            if(!client->parent.isEmpty()){
                /*如果自己是子窗口*/
                for(auto cefClient=m_cefClients.begin();cefClient!=m_cefClients.constEnd();++cefClient){
                    if(cefClient.value().parent==client->parent&&cefClient.value().needHide&&cefClient->objectName!=objectName){
                        cefClient->root->setVisible(false);
                        cefClient->isShow=false;
                    }
                }
            }else{
                /*如果自己是父窗口*/
                for(auto child:client->children){
                    for(auto cefClient=m_cefClients.begin();cefClient!=m_cefClients.constEnd();++cefClient){
                        if(cefClient.value().objectName==child){
                            cefClient->root->setVisible(false);
                            cefClient->isShow=false;
                            break;
                        }
                    }
                }
            }
        }
        if(client->root->visibility()==QWindow::Hidden||client->root->visibility()==QWindow::Minimized){
            client->root->setVisibility(QWindow::AutomaticVisibility);
            client->isShow=true;
        }
        client->root->raise();
        /*置顶强制置顶的窗口*/
        frontWindow(client->identifier);
        /*如果父窗口不为空，将自己排在父窗口结构体子窗口队列的第一位*/
        if(!client->parent.isEmpty()){
            if(m_cefClients.contains(client->parent)){
                CefClientStruct *cefClient= &m_cefClients[client->parent];
                cefClient->children.removeOne(objectName);
                cefClient->children.push_front(objectName);
            }
        }
    }
}

void QCefView::moveWindow(QString objectName)
{
    if(m_cefClients.contains(objectName)){
        CefClientStruct *cefClient= &m_cefClients[objectName];
        MouseKeyHook::getInstance().moveWindow(cefClient);
    }
}

void QCefView::resizeWindow(QString objectName, QString width, QString height)
{
    if(m_cefClients.contains(objectName)){
        CefClientStruct *cefClient= &m_cefClients[objectName];
        int i_width=0;
        int i_height=0;
        if(width=="1"){
            i_width=cefClient->root->width();
        }
        if(height=="1"){
            i_height=cefClient->root->height();
        }
        MouseKeyHook::getInstance().resizeWindow(cefClient,i_width,i_height);
    }
}

QString QCefView::getCefClients(QString objectName,bool containSelf)
{
    if(m_cefClients.contains(objectName)){
        CefClientStruct *cefClient= &m_cefClients[objectName];
        QStringList childrenList;
        /*如果父窗口存在，就发送同级窗口*/
        if(!cefClient->parent.isEmpty()){
            CefClientStruct *client= &m_cefClients[cefClient->parent];
            childrenList=client->children;
        }
        else{
            childrenList=cefClient->children;
        }
        if(!containSelf){  //如果不包含自己 删掉自己
            childrenList.removeOne(objectName);
        }
        return "["+childrenList.join(',')+"]";
    }
    return "[]";
}

QString QCefView::getFatherName(QString objectName)
{
    if(m_cefClients.contains(objectName)){
        CefClientStruct *cefClient= &m_cefClients[objectName];
        if(cefClient->parent.isEmpty()){
            return objectName;
        }
        return cefClient->parent;
    }
    return "";
}

void QCefView::test()
{


}

CefClientStruct *QCefView::getCefClientStruct(QString objectName)
{
    if(m_cefClients.contains(objectName)){
        return &m_cefClients[objectName];
    }
    return nullptr;
}

void QCefView::setHookPopup(QString objectName)
{
    m_HookPopupName=objectName;
}


void QCefView::openUrl(QString url)
{
    QDesktopServices::openUrl(QUrl(url));
}

void QCefView::reload(QString objectName)
{
    if(m_cefClients.contains(objectName)){
        CefClientStruct *cefClient=&m_cefClients[objectName];
        if(cefClient->client){
            cefClient->client->browser()->ReloadIgnoreCache();
        }
    }
}

void QCefView::load(QString objectName, QString url)
{
    if(m_cefClients.contains(objectName)){
        CefClientStruct *cefClient=&m_cefClients[objectName];
        if(cefClient->client){
            cefClient->client->browser()->GetMainFrame()->LoadURL(url.toStdString());
        }
    }
}

void QCefView::frontWindow(int identifier)
{
    for(auto cefClient=m_cefClients.begin();cefClient!=m_cefClients.constEnd();++cefClient){
        if(cefClient.value().isFront&&cefClient.value().client->browser()->GetIdentifier()!=identifier){
            cefClient.value().root->raise();
        }
    }
}

bool QCefView::getNeedHide(QString objectName)
{
    if(m_cefClients.contains(objectName)){
        CefClientStruct *cefClient=&m_cefClients[objectName];
        return cefClient->needHide;
    }
    return false;
}


void QCefView::onCloseBrowser(CefRefPtr<CefBrowser> browser)
{
    QString objectName;
    for(auto cefClient=m_cefClients.begin();cefClient!=m_cefClients.constEnd();++cefClient){
        if(cefClient.value().identifier==browser->GetIdentifier()){
            objectName=cefClient.value().objectName;
            if(!cefClient.value().parent.isEmpty()){
                /*在父窗口队列里删掉自己*/
                CefClientStruct* client=&m_cefClients[cefClient.value().parent];
                client->children.removeOne(cefClient.value().objectName);
            }
            emit sgCloseWindow(cefClient.value().objectName);
            break;
        }
    }
    if(!objectName.isEmpty()){
        m_cefClients.remove(objectName);
    }
}


void QCefView::onSendCookie(QString cookieName,QString cookie)
{
    cookies[cookieName]=cookie;
}

void QCefView::onDownload(CefRefPtr<CefBrowser> browser, int progress, QString state, QString fileName)
{
    for(auto cefClient=m_cefClients.begin();cefClient!=m_cefClients.constEnd();++cefClient){
        if(cefClient.value().identifier==browser->GetIdentifier()){
            QString msg=QString("qtSlots({\"Download\":\"%1\",").arg(fileName);

            if(state=="Downloading"){
                msg=msg+QString("\"Progress\":\"%1\"})").arg(QString::number(progress));
            }
            if(state=="Succeed"){
                msg=msg+QString("\"State\":\"Succeed\"})");
            }
            this->sendToWeb(cefClient.value().objectName,msg);
        }
    }
}

void QCefView::onLoadEnd(CefRefPtr<CefBrowser> browser)
{

}


void QCefView::closeAllBrowser()
{
    QStringList m_cefClients_key=m_cefClients.keys();
    foreach(auto name,m_cefClients_key){
        closeWindow(name);
    }
}
