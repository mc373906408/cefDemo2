#include <QDebug>
#include "QCefContext.h"
#include <QCoreApplication>
#include <QDir>

int QCefContext::initCef(int& argc, char** argv)
{
    // 创建CEF默认命令行参数.
    m_cmdLine = CefCommandLine::CreateCommandLine();
    HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(NULL);
    CefMainArgs mainArgs(hInstance);

    m_cmdLine->InitFromString(GetCommandLineW());
    return initCef(mainArgs);
}

int QCefContext::initCef(CefMainArgs& mainArgs)
{
    CefRefPtr<CefApp> app(new QCefApp);
    // 创建一个正确类型的App Client

    m_cefApp = CefRefPtr<QCefApp>((QCefApp*)app.get());

    CefSettings settings;
    /*禁用cef日记*/
    settings.log_severity = LOGSEVERITY_DISABLE;
    /*web控制台端口*/
    settings.remote_debugging_port=10001;

    /*渲染器位置*/
    CefString(&settings.browser_subprocess_path).FromString(QString(QCoreApplication::applicationDirPath()+"/CefClient_render").toStdString());
    QDir dir(QCoreApplication::applicationDirPath());
    if(!dir.exists("cache")){
        dir.mkdir("cache");
    }
    /*缓存位置*/
    CefString(&settings.cache_path).FromString(QString(QCoreApplication::applicationDirPath()+"/cache").toStdString());



    // 初始化CEF
    CefInitialize(mainArgs, settings, app.get(), NULL);
    return -1;
}

