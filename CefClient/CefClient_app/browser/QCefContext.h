#ifndef QCEFCONTEXT_H
#define QCEFCONTEXT_H

#include "QCefApp.h"

class QCefContext
{
public:

    //初始化 Cef
    int initCef(int &argc, char** argv);

public:
    CefRefPtr<QCefApp> cefApp() {return m_cefApp;}
private:
    int initCef(CefMainArgs& mainArgs);
private:
    CefRefPtr<QCefApp> m_cefApp=nullptr;
#if defined(OS_WIN)
    CefRefPtr<CefCommandLine> m_cmdLine=nullptr;
#endif
};



#endif //QCEFCONTEXT_H
