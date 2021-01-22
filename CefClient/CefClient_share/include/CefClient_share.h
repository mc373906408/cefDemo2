#ifndef CEFCLIENT_SHARE_H
#define CEFCLIENT_SHARE_H

#include "global/CefClient_share_global.h"
#include "Tools/Tools.h"

class CEFCLIENT_SHARE_EXPORT CefClientShare
{
public:
    static CefClientShare& getInstance(){
       static CefClientShare instance;
       return instance;
     }

    Tools* getTools();

private:
    CefClientShare();
    ~CefClientShare();

    CefClientShare(const CefClientShare &sg) = delete;
    CefClientShare &operator=(const CefClientShare &sg) = delete;

private:
    Tools *m_Tools;
};

#endif // CEFCLIENT_SHARE_H
