#ifndef QCEFCLIENT_H
#define QCEFCLIENT_H

#include <QObject>
#include <QDebug>
#include <QWindow>

#include "include/cef_client.h"
#include "include/cef_life_span_handler.h"
#include "include/wrapper/cef_helpers.h"
#include "include/cef_resource_request_handler.h"

class QCefView;


class QCefClient:
        public QObject,
        public CefClient,
        public CefLifeSpanHandler,
        public CefLoadHandler,
        public CefKeyboardHandler,
        public CefContextMenuHandler,
        public CefDownloadHandler,
        public CefCookieVisitor,
        public CefFocusHandler
{
    Q_OBJECT
public:
    /*注册回调接口*/
    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;
    virtual CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() override;
    virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override;
    virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() override;
    virtual CefRefPtr<CefDownloadHandler> GetDownloadHandler() override;
    virtual CefRefPtr<CefFocusHandler> GetFocusHandler() override;

    /*浏览器创建成功*/
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
    /*浏览器关闭*/
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;
    /*收到H5信息*/
    virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) override;
    /*按键触发*/
    virtual bool OnPreKeyEvent(CefRefPtr<CefBrowser> browser, const CefKeyEvent &event, MSG *os_event, bool *is_keyboard_shortcut) override;
    /*网页加载失败*/
    virtual void OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString &errorText, const CefString &failedUrl) override;
    /*网页加载成功*/
    virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) override;
    /*禁用右键菜单*/
    virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model) override;
    /*H5下载*/
    virtual void OnBeforeDownload(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDownloadItem> download_item, const CefString &suggested_name, CefRefPtr<CefBeforeDownloadCallback> callback) override;
    /*H5下载状态*/
    virtual void OnDownloadUpdated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDownloadItem> download_item, CefRefPtr<CefDownloadItemCallback> callback) override;
    /*获取cookie*/
    virtual bool Visit(const CefCookie &cookie, int count, int total, bool &deleteCookie) override;
    /*拦截弹出窗口*/
    virtual bool OnBeforePopup(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString &target_url, const CefString &target_frame_name, WindowOpenDisposition target_disposition, bool user_gesture, const CefPopupFeatures &popupFeatures, CefWindowInfo &windowInfo, CefRefPtr<CefClient> &client, CefBrowserSettings &settings, CefRefPtr<CefDictionaryValue> &extra_info, bool *no_javascript_access) override;
    /*获取焦点*/
    virtual void OnGotFocus(CefRefPtr<CefBrowser> browser) override;
public:
    QCefClient(QCefView *cefView);
    ~QCefClient(){}
    CefRefPtr<CefBrowser> browser();
#ifdef Q_OS_WIN
    CefWindowHandle browserWinId();
#endif


signals:
    /*H5信息*/
    void sgWebMsgReceived(QString msg);
    /*关闭标签页*/
    void sgCloseBrowser(CefRefPtr<CefBrowser> browser);
    /*发送cookie*/
    void sgSendCookie(QString cookieName,QString cookie);
    /*发送下载进度和状态*/
    void sgDownload(CefRefPtr<CefBrowser> browser,int progress,QString state,QString fileName);
    /*发送页面加载完成*/
    void sgLoadEnd(CefRefPtr<CefBrowser> browser);
private:
    bool m_created=false;
    bool m_closeLoadError=false;
    int m_progress=0;
    CefRefPtr<CefBrowser> m_browser=nullptr;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(QCefClient);


    int mainUrlSwitchCount=0;

};


#endif //QCEFCLIENT_H
