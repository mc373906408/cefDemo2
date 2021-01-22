#ifndef QCEFAPP_H
#define QCEFAPP_H

#include "QCefClient.h"
#include <QList>
#include <QQueue>
#include <QDebug>

#include "include/cef_app.h"
#include "include/cef_request_context_handler.h"


class QCefView;


class ClientRequestContextHandler:public CefRequestContextHandler{
public:
    ClientRequestContextHandler() {}
    virtual bool OnBeforePluginLoad(const CefString& mime_type,
                            const CefString& plugin_url,
                            bool is_main_frame,
                            const CefString& top_origin_url,
                            CefRefPtr<CefWebPluginInfo> plugin_info,
                            PluginPolicy* plugin_policy) OVERRIDE {
        *plugin_policy=PLUGIN_POLICY_ALLOW;
        return true;
    }


private:
    IMPLEMENT_REFCOUNTING(ClientRequestContextHandler);
    DISALLOW_COPY_AND_ASSIGN(ClientRequestContextHandler);
};

class QCefApp:
public CefApp,
public CefBrowserProcessHandler
{
public:
    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override;
    virtual void OnContextInitialized() override;
    virtual void OnBeforeCommandLineProcessing(const CefString &process_type, CefRefPtr<CefCommandLine> command_line) override;
public:
    QCefApp(){}
    virtual ~QCefApp(){}

    /*创建浏览器进程的工厂方法*/
    CefRefPtr<QCefClient> addBrowser(QCefView *cefView,QString title,bool pop,QString url,int width=0,int height=0,int x=0,int y=0,QWindow* window=nullptr);

    /*关闭所有浏览器进程*/
    void closeAllBrowser();

    /*关闭指定id的浏览器进程*/
    void closeBrowser(int id);

private:
    bool m_contextReady;
    QQueue<CefRefPtr<QCefClient>> m_clients;
//    CefRefPtr<QCookieVisitor> m_cookieVisitor;
    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(QCefApp);
};

#endif //QCEFAPP_H
