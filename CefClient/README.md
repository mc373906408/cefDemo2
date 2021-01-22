# CefClient
本项目为QT+QML+CEF的DEMO

## 实现功能
- 创建浏览器窗口加载页面
- 页面与C++之间通讯
- windows父子窗口特性实现窗口叠加
- 拖动、拉伸
- 模块化加载多项目结构

## 构造环境
- 以下环境都是x64
- QT5.12以上，推荐QT5.15
- CMake3.15以上
- VS2017以上

## 构造步骤
- CMake GUI 加载项目根目录CMakeLists.txt
- 或者Qt Creator 加载项目根目录CMakeLists.txt

## 构造目标
- CefClient_app  Release(不可调试) 或  RelWithDebInfo(VS可调试)

## 子项目简述
### CefClient_app
主程序，所有UI、窗口创建、cef事件循环
### CefClient_render
cef渲染器，每启动一个标签页都会自启动本进程实例，本进程负责与H5通讯
### CefClient_share
动态库，实现模块化加载，当前只放置工具类函数