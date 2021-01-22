#include "QCefApp.h"
#include <QCoreApplication>
#include <QDebug>


CefRefPtr<CefBrowserProcessHandler> QCefApp::GetBrowserProcessHandler()
{
    return this;
}

void QCefApp::OnContextInitialized()
{
//    CEF_REQUIRE_UI_THREAD();
    m_contextReady=true;
}

void QCefApp::OnBeforeCommandLineProcessing(const CefString &process_type, CefRefPtr<CefCommandLine> command_line)
{
    /*浏览器设置
    command_line->AppendSwitchWithValue("--force-device-scale-factor","2");
    command_line->AppendSwitch();
    command_line->AppendSwitchWithValue("type","renderer");
    */
    /*https链接可以访问http的js*/
    command_line->AppendSwitch("--allow-running-insecure-content");
    /*关闭同源策略,跨域*/
    command_line->AppendSwitch("--disable-web-security");
    /*禁用代理*/
//    command_line->AppendSwitch("no-proxy-server");
    /*忽略ssl证书错误*/
    command_line->AppendSwitchWithValue("--ignore-urlfetcher-cert-requests","1");
    command_line->AppendSwitchWithValue("--ignore-certificate-errors","1");
    /*启动自动播放*/
    command_line->AppendSwitchWithValue("autoplay-policy", "no-user-gesture-required");
}

#if defined(OS_WIN)
CefRefPtr<QCefClient> QCefApp::addBrowser(QCefView *cefView,QString title,bool pop,QString url,int width,int height,int x,int y,QWindow* window)
{
    if(m_contextReady)
    {
        // 创建本地窗口所需的信息
        CefWindowInfo windowInfo;

        // 针对Windows系统，需要指定特殊的标识，
        // 这个标识会被传递给CreateWindowEx()方法
        windowInfo.SetAsPopup(NULL, title.toStdString());
        /*设置窗口风格，初始值不可见，指定父窗口后设置可见*/
        if(!pop)
            windowInfo.style=WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
        if(width!=0)
            windowInfo.width=width;
        if(height!=0)
            windowInfo.height=height;

        // 初始化cef client方法
        CefRefPtr<QCefClient> client(new QCefClient(cefView));
        // 指定CEF浏览器设置
        CefBrowserSettings browserSettings;


        /*启动flash*/
        CefRequestContextSettings rcsettings;
        /*指定缓存路径，存cookie*/
        CefString(&rcsettings.cache_path).FromString(QString(QCoreApplication::applicationDirPath()+"/cache").toStdString().c_str());
        CefRefPtr<CefRequestContext> request_content=CefRequestContext::CreateContext(rcsettings, new ClientRequestContextHandler);
        CefString error;
        CefRefPtr<CefValue> value = CefValue::Create();
        value->SetInt(1);
        request_content->SetPreference("profile.default_content_setting_values.plugins", value, error);
        // 创建浏览器窗口
        CefBrowserHost::CreateBrowser(windowInfo, client.get(), url.toStdString(), browserSettings, nullptr,request_content);
        // 将浏览器引用添加到浏览器队列
        m_clients.enqueue(client);
        return client;
    }
    return nullptr;
}
#endif

void QCefApp::closeAllBrowser()
{
    while (!m_clients.empty())
    {
        m_clients.dequeue()->browser()->GetHost()->CloseBrowser(true);
    }
}

void QCefApp::closeBrowser(int id)
{
    for(int i=0;i<m_clients.size();i++)
    {
        if(m_clients.at(i)->browser()->GetIdentifier()==id)
        {
            m_clients.at(i)->browser()->GetHost()->CloseBrowser(true);
            m_clients.removeAt(i);
            break;
        }
    }
}

