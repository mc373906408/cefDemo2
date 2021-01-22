@echo off

set RunTarget=%1
set ProjectRoot=%2
set CurrentPath=%3
set QtPath=%4

if not exist %CurrentPath%/%RunTarget%.exe goto end

@REM 拷贝cef
xcopy "%ProjectRoot%/3rd/cef/bin" "%CurrentPath%" /E /Y

@REM 拷贝qt
%QtPath%/bin/windeployqt.exe  "%CurrentPath%/%RunTarget%.exe" --qmldir "%ProjectRoot%/%RunTarget%/qml"

@REM 结束
:end