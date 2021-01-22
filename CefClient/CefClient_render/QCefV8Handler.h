#ifndef QCEFV8HANDLER_H
#define QCEFV8HANDLER_H

#include "include/cef_v8.h"

class QCefV8Handler:public CefV8Handler
{
public:
    virtual bool Execute(const CefString &name, CefRefPtr<CefV8Value> object, const CefV8ValueList &arguments, CefRefPtr<CefV8Value> &retval, CefString &exception) override;
public:
    QCefV8Handler(CefRefPtr<CefBrowser> browser);

private:
    CefRefPtr<CefBrowser> m_browser;
    IMPLEMENT_REFCOUNTING(QCefV8Handler);
};

#endif
