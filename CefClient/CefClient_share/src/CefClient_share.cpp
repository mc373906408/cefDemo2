#include "include/CefClient_share.h"

#include <QDebug>

CefClientShare::CefClientShare()
{

}

CefClientShare::~CefClientShare()
{
    if(m_Tools){
        delete m_Tools;
        m_Tools=nullptr;
    }
}

Tools* CefClientShare::getTools()
{
    if(!m_Tools){
        m_Tools=new Tools();
    }
    if(m_Tools){
        return m_Tools;
    }else{
        return nullptr;
    }
}
