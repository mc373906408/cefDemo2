# cmake最低版本号
cmake_minimum_required(VERSION 3.15)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

set(TARGET "CefClient_render")

# 引入QT依赖包，同时需要修改target_link_libraries
find_package(Qt5 COMPONENTS Core  REQUIRED)

# 源文件  cpp\h\qrc\hpp\c
add_executable(${TARGET}
    main.cpp
    QCefRenderHandler.h
    QCefRenderHandler.cpp
    QCefV8Handler.h
    QCefV8Handler.cpp
    )

# 宏定义 具体是干啥不知道，自动生成的cmake中有
target_compile_definitions(${TARGET}
    PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

# 将目标文件与库文件进行链接
target_link_libraries(${TARGET}
     Qt5::Core)

# 导入cef
include_directories(${CEF_INCLUDE_DIR})
target_link_libraries(${TARGET} cef_sandbox libcef libcef_dll_wrapper)

