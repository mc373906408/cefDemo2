#ifndef QCEFRENDERHANDLER_H
#define QCEFRENDERHANDLER_H

#include "include/cef_app.h"
#include "QCefV8Handler.h"

class QCefRenderHandler:
        public CefApp,
        public CefRenderProcessHandler
{
public:
    virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override;
    virtual void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) override;
    virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) override;
private:
    CefRefPtr<QCefV8Handler> m_v8Handler;

    IMPLEMENT_REFCOUNTING(QCefRenderHandler);
};

#endif //QCEFRENDERHANDLER_H
