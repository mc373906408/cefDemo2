#include "QCefRenderHandler.h"
#include <QDebug>
#include <QFile>


CefRefPtr<CefRenderProcessHandler> QCefRenderHandler::GetRenderProcessHandler()
{
    return this;
}

void QCefRenderHandler::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
    // 取回V8上下文的window对象
    CefRefPtr<CefV8Value> object = context->GetGlobal();

    // 创建"sendMessage"函数，作为消息通道使用.
    m_v8Handler = CefRefPtr<QCefV8Handler>(new QCefV8Handler(browser));
    CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction("callQML", m_v8Handler);

    // 将消息通道注册到window对象
    object->SetValue("callQML", func, V8_PROPERTY_ATTRIBUTE_NONE);
}


bool QCefRenderHandler::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
{
    //收到来自Browser进程发过来的信息
//    QFile file("in.txt");
//    file.open(QIODevice::WriteOnly);
//    file.write(message->GetName().ToString().c_str());
    frame->ExecuteJavaScript(message->GetName(),frame->GetURL(),0);
    return true;
}
