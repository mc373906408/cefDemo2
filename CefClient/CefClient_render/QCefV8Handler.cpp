#include "QCefV8Handler.h"
#include <QDebug>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>

QCefV8Handler::QCefV8Handler(CefRefPtr<CefBrowser> browser)
{
    m_browser=browser;
}

bool QCefV8Handler::Execute(const CefString &name, CefRefPtr<CefV8Value> object, const CefV8ValueList &arguments, CefRefPtr<CefV8Value> &retval, CefString &exception)
{
    if (name=="callQML")
    {
        if (arguments.size() == 3)
        {
            //这里收到了H5的请求
            CefString msgName = arguments.at(0)->GetStringValue();
            CefString msgId = arguments.at(1)->GetStringValue();
            CefString msgStr = arguments.at(2)->GetStringValue();
            QJsonObject json;
            json.insert("msgName","callQML");
            json.insert("name",msgName.ToString().c_str());
            json.insert("id",msgId.ToString().c_str());
            json.insert("request",msgStr.ToString().c_str());
            QString str=QString(QJsonDocument(json).toJson());
            //消息会被发送到CefClient的OnProcessMessageReceived接口方法
            m_browser->GetMainFrame()->SendProcessMessage(PID_BROWSER, CefProcessMessage::Create(str.toStdString().c_str()));
            retval = CefV8Value::CreateInt(0);
        }
        return true;
    }
    return false;
}
