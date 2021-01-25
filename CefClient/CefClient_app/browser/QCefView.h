#ifndef QCEFVIEW_H
#define QCEFVIEW_H

#include <QTimer>
#include <QWindow>
#include <QJsonObject>

#include "include/cef_app.h"

#include "QCefContext.h"
#include "QCefClient.h"



struct CefClientStruct{
    QString objectName="";
    CefRefPtr<QCefClient> client;  //cef智能指针
    int identifier;  //cef唯一id
    QWindow* root=nullptr;  //本窗口指针
    QObject* father=nullptr; //父窗口指针
    QWindow* cefWnd;  //cef子窗口指针
    int width;
    int height;
    int beginWidth; //初始宽度，用于计算子窗口位置
    int beginHeight; //初始高度，用于计算子窗口位置
    QString parent; //父窗口objectName
    int parent_left; //与父窗口的左边距
    int parent_top; //与父窗口的上边距
    int parent_right; //与父窗口的右边距
    int parent_bottom; //与父窗口的下边距
    QString url;
    QPoint clickPos; //位移开始时鼠标位置
    QStringList children; //子窗口列表
    bool needHide=false;  //切换标签隐藏
    bool lostFocusHide=false; //失去焦点隐藏
    int win_x;
    int win_y;
    int win_width;
    int win_height;
    bool isMax=false;
    bool isFull=false;
    bool isFront=false;
    bool isShow=true;  //用于修复自动窗口显示bug
    QJsonObject json;
};


class QCefView:public QObject
{
    Q_OBJECT
    enum Metamorphosis{
        X,
        Y,
        Width,
        Height
    };

public:
    static QCefView *getInstance(){
        if(instance==nullptr){
            instance=new QCefView();
        }
        return instance;
    }

    /*初始化*/
    void init(QWindow *window,QCefContext *cef);

    /*关闭所有网页*/
    Q_INVOKABLE void closeAllBrowser();

    /*给指定H5发请求*/
    Q_INVOKABLE void sendToWeb(QString objectName,QString msg);

    /*修改CEF窗口状态*/
    Q_INVOKABLE void setWindowStatus(QString objectName,QString status);

    /*还原全屏*/
    void setWindowFillScreen();

    /*关闭指定窗口*/
    Q_INVOKABLE void closeWindow(QString objectName);

    /*打开网页*/
    Q_INVOKABLE void openUrl(const QString &objectName/*当前窗口名字*/,const QString &fatherName/*父窗口名字*/,const QString &url/*打开的网页*/,const QString &rect/*窗口宽高，初始位置*/,const QString &align/*水平居中，设置这个属性后rect初始位置失效*/,const QString &valign/*上下居中，设置这个属性后rect初始位置失效*/,const QString &needHide/*打开/切换其他标签页时隐藏*/,const QString &lostFocusHide/*失去焦点隐藏*/,const QString &front/*是否置顶*/,const bool &visible=true/*初始是否隐藏*/);

    /*打开窗口*/
    Q_INVOKABLE void openWindow(const QString &objectName/*当前窗口名字*/,const QString &fatherName/*父窗口名字*/,const QString &rect/*窗口宽高，初始位置*/,const QString &align/*水平居中，设置这个属性后rect初始位置失效*/,const QString &valign/*上下居中，设置这个属性后rect初始位置失效*/,const QString &needHide/*打开/切换其他标签页时隐藏*/,const QString &lostFocusHide/*失去焦点隐藏*/,const QString &front/*是否置顶*/,const bool &visible=true/*初始是否隐藏*/);

    /*切换窗口*/
    Q_INVOKABLE void switchWindow(QString objectName,QString needHide);

    /*移动指定窗口*/
    Q_INVOKABLE void moveWindow(QString objectName);

    /*拉伸指定窗口*/
    Q_INVOKABLE void resizeWindow(QString objectName,QString width,QString height);

    /*获取当前窗口的同级窗口，如果是顶级窗口，返回子窗口列表*/
    Q_INVOKABLE QString getCefClients(QString objectName,bool containSelf=true);

    /*获取父窗口name*/
    Q_INVOKABLE QString getFatherName(QString objectName);

    /*测试接口*/
    Q_INVOKABLE void test();

    /*获取窗口指针*/
    CefClientStruct *getCefClientStruct(QString objectName);

    /*设置拦截网址*/
    Q_INVOKABLE void setHookPopup(QString objectName);


    /*打开外部浏览器*/
    Q_INVOKABLE void openUrl(QString url);

    /*强制刷新*/
    Q_INVOKABLE void reload(QString objectName);

    /*重定向*/
    Q_INVOKABLE void load(QString objectName,QString url);

    /*置顶窗口*/
    void frontWindow(int identifier);

    /*判断是否为标签页*/
    Q_INVOKABLE bool getNeedHide(QString objectName);





    /*启动cef事件循环*/
    void doEveryLoop();

signals:
    void sgWebMsgReceived(QString msg);

    void sgOpenNewWindow(QObject* father,const QString &json);
    void sgCloseWindow(const QString &name);
    void sgCloseLogo();

public slots:
    void onCloseBrowser(CefRefPtr<CefBrowser> browser);
    void onSendCookie(QString cookieName,QString cookie);
    void onDownload(CefRefPtr<CefBrowser> browser,int progress,QString state,QString fileName);
    void onLoadEnd(CefRefPtr<CefBrowser> browser);

protected slots:
    virtual void onCefTimer();


public:
    bool ismove=false;

    QString m_HookPopupName;
    QStringList m_HookPopup;
private:
    QCefView();
    ~QCefView();

    QCefView(const QCefView &) = delete;
    QCefView &operator =(const QCefView &) = delete;

    static QCefView *instance;

    class GC // 垃圾回收类
    {
    public:
        GC()
        {
        }
        ~GC()
        {
            // We can destory all the resouce here, eg:db connector, file handle and so on
            if (instance != nullptr)
            {
                delete instance;
                instance = nullptr;
            }
        }
    };
    static GC gc;  //垃圾回收类的静态成员


    void windowMetamorphosis(CefClientStruct *cefClient,Metamorphosis mode);

    QJsonObject getWindowJson(const QString &objectName/*当前窗口名字*/,const QString &fatherName/*父窗口名字*/,const QString &rect/*窗口宽高，初始位置*/,const QString &align/*水平居中，设置这个属性后rect初始位置失效*/,const QString &valign/*上下居中，设置这个属性后rect初始位置失效*/,const QString &lostFocusHide/*失去焦点隐藏*/,const QString &front/*是否置顶*/,const bool &visible/*初始是否隐藏*/);

    CefClientStruct getWindowCefClientStruct(const QString &objectName, const QString &fatherName, const QString &rect, const QString &align, const QString &valign, const QString &needHide, const QString &lostFocusHide, const QString &front, const bool &visible);
private:
    QTimer *m_cefTimer=nullptr;
    QWindow *m_window=nullptr;
    QCefContext *m_cef=nullptr;
    QHash<QString,QString> cookies;
    QHash<QString, CefClientStruct> m_cefClients;
    QList<QPair<QString/*objectName*/,bool/*openCef*/>> m_cefClientsOpenList;
};


#endif //QCEFVIEW_H
