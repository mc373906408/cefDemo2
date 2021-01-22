# cmake最低版本号
cmake_minimum_required(VERSION 3.15)

set(TARGET "CefClient_share")

find_package(Qt5 COMPONENTS Core Quick REQUIRED)

add_library(${TARGET} SHARED
include/global/CefClient_share_global.h
include/CefClient_share.h
src/CefClient_share.cpp
include/Tools/Tools.h
src/Tools/Tools.cpp
)

include_directories(include)

target_link_libraries(${TARGET} PRIVATE Qt5::Core Qt5::Quick)

# 宏定义 具体是干啥不知道，自动生成的cmake中有
target_compile_definitions(${TARGET} PRIVATE CEFCLIENT_SHARE_LIBRARY)


