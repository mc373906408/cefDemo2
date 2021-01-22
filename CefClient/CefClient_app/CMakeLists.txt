# cmake最低版本号
cmake_minimum_required(VERSION 3.15)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

set(TARGET "CefClient_app")

# 引入QT依赖包，同时需要修改target_link_libraries
find_package(Qt5 COMPONENTS Core Quick QuickWidgets REQUIRED)

# 源文件  cpp\h\qrc\hpp\c
add_executable(${TARGET}
    main.cpp
    qml/qml.qrc
    browser/QCefApp.h
    browser/QCefApp.cpp
    browser/QCefClient.h
    browser/QCefClient.cpp
    browser/QCefContext.h
    browser/QCefContext.cpp
    browser/QCefView.h
    browser/QCefView.cpp
    windowsHook/MouseKeyHook.h
    windowsHook/MouseKeyHook.cpp
    )

# 宏定义 具体是干啥不知道，自动生成的cmake中有
target_compile_definitions(${TARGET}
    PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

# 将目标文件与库文件进行链接
target_link_libraries(${TARGET}
     Qt5::Core Qt5::Quick Qt5::QuickWidgets)

# 导入cef
include_directories(${CEF_INCLUDE_DIR})
target_link_libraries(${TARGET} cef_sandbox libcef libcef_dll_wrapper)

# 导入cefclient_share
include_directories(${CEF_CLIENT_SHARE_INCLUDE_DIR})
target_link_libraries(${TARGET} CefClient_share)

# 导入qhook
include_directories(${QHOOK_INCLUDE_DIR})
target_link_libraries(${TARGET} QHook)

# vs中显示qml文件
file(GLOB_RECURSE QML_FILES "qml/*.qml")
target_sources(${TARGET} PRIVATE ${QML_FILES})

# 执行编译后拷贝脚本,这里代码很丑陋，重复了
add_custom_command(TARGET ${TARGET}
    POST_BUILD
    COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO}/copyAll.cmd
    ARGS ${TARGET} ${PROJECT_ROOT} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO} ${QT_PATH}
    )

add_custom_command(TARGET ${TARGET}
    POST_BUILD
    COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/copyAll.cmd
    ARGS ${TARGET} ${PROJECT_ROOT} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE} ${QT_PATH}
    )
