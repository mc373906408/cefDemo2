#include <QQuickView>
#include <QApplication>
#include <QQmlContext>


#include <Windows.h>

#include "browser/QCefApp.h"
#include "browser/QCefView.h"

#include "windowsHook/MouseKeyHook.h"

#include "CefClient_share.h"


QCefView *QCefView::instance=nullptr;
QCefView::GC QCefView::gc;  //类的静态成员需要类外部初始化，这一点很重要，否则程序运行连GC的构造都不会进入，何谈自动析构

int main(int argc, char *argv[])
{
    /*禁用DPI缩放，自动DPI缩放会引起CEF网页异常，所有缩放都手动乘上DPI倍数*/
    SetProcessDPIAware();

    QApplication app(argc,argv);
    QQuickView qmlView;

	qmlView.setFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint | Qt::WindowMinimizeButtonHint);
	qmlView.setX(0);
	qmlView.setY(0);


    qmlView.rootContext()->setContextProperty("cppView",&qmlView);
    qmlView.rootContext()->setContextProperty("cppCef",QCefView::getInstance());

    qmlView.setSource(QUrl("qrc:/main.qml"));

    /*初始化键盘鼠标hook*/
    MouseKeyHook::getInstance();

    QCefContext cef;

    int rv=0;
    rv=cef.initCef(argc,argv);

    if (rv>=0){
        return 0;
    }
    QCefView::getInstance()->init(&qmlView,&cef);

    QUrl url(QCoreApplication::applicationDirPath()+"/html/404.html");
    QCefView::getInstance()->openUrl("404","",url.toString(),"0,0,800,800","Center","VCenter","","","");

    qmlView.show();
    QCefView::getInstance()->doEveryLoop();

    rv=app.exec();
    return rv;
}
