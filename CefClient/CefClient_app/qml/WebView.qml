import QtQuick 2.0

Item {

    property alias jsobject: m_object
    QtObject {
        id:m_object
        /* m_object在H5中是一个获取标示 */
        objectName: "m_object"

        /* @id:作为消息id，递增，@request：使用MsgId.id_xxx()获取，@msg：成功信息（一般为success） */
        function createSuccessMsg(id,request,msg) {
            var ret = {
                "request":request,
                "status":"success",
                "id":id,
                "successMsg":msg
            };
            return ret;
        }

        /* @id:作为消息id，递增，@request：使用MsgId.id_xxx()获取，@failureId:失败id（暂未规定），@msg：失败信息 */
        function createFailureMsg(id,request,failureId,msg) {
            var ret = {
                "request":request,
                "statuc":"failure",
                "id":id,
                "failureVal":failureId,
                "failureMsg":msg
            }
            return ret;
        }

        function callback(name,msg){
            cppCef.sendToWeb(name,"qtQueryCallback("+JSON.stringify(msg)+")")
        }
        function callWeb(name,msg){
            cppCef.sendToWeb(name,"qtSlots("+JSON.stringify(msg)+")")
        }

        function callQML(msgStr) {
            // Process some thing
            var str = JSON.parse(msgStr);
            var name=str["name"]
            var request = str["request"];

            var json=JSON.parse(request)
//            cppLog.netInfo("name:"+name+" recv H5:"+request)
            console.log("xxx "+"name:"+name+" recv H5:"+request)

            /* 收到H5发来的信息 */
            var ret;
            var fatherName;
            if(json["WindowStatus"]!==undefined){
                cppCef.setWindowStatus(name,json["WindowStatus"])
            }
            if(json["Close"]!==undefined){
                fatherName =cppCef.getFatherName(name)
                var callbackmsg= cppCef.getCefClients(json["Close"],false)
                var needHide=cppCef.getNeedHide(json["Close"])
                callback(name,createSuccessMsg(str.id,request,callbackmsg))
                if(needHide){
                    ret={"OpenChildren":callbackmsg,"BeName":name}
                    callWeb(fatherName,ret)
                }
                cppCef.closeWindow(json["Close"])
            }
            if(json["Open"]!==undefined){
                cppCef.openWindow(json["Open"],json["Father"],json["IE"],json["Url"],json["RECT"],json["Align"],json["VAlign"],json["NeedHide"],json["LostFocusHide"],json["Front"])
                callback(name,createSuccessMsg(str.id,request,cppCef.getCefClients(json["Father"])))
                /*打开菜单不发送*/
                if(json["LostFocusHide"]!=="1"){
                    ret={"OpenChildren":cppCef.getCefClients(json["Father"]),"BeName":name}
                    callWeb(json["Father"],ret)
                }
            }
            if(json["SwitchWindow"]!==undefined){
                fatherName=cppCef.getFatherName(name)
                cppCef.switchWindow(json["SwitchWindow"],json["NeedHide"])
                callback(name,createSuccessMsg(str.id,request,cppCef.getCefClients(fatherName)))
            }
            if(json["MouseDown"]!==undefined){
                cppCef.moveWindow(json["MouseDown"])
            }
            if(json["GetCefClients"]!==undefined){
                callback(name,createSuccessMsg(str.id,request,cppCef.getCefClients(json["GetCefClients"])))
            }
            if(json["SendMsg"]!==undefined){
                callWeb(json["SendMsg"],json["Msg"])
            }
            if(json["ClearCache"]!==undefined){
                cppCef.clearIECache()
                cppConfig.setIsCache(true)
                cppTool.quitApp()
            }
            if(json["SetMute"]!==undefined){
                if(json["SetMute"]==="1"){
                    cppTool.setMute(true)
                }else{
                    cppTool.setMute(false)
                }
            }
            if(json["StartProcess"]!==undefined){
                cppTool.startLocalProcess(json["StartProcess"])
            }
            if(json["QuitApp"]!==undefined){
                cppTool.quitApp()
            }
            if(json["SetHookPopup"]!==undefined){
                cppCef.setHookPopup(json["SetHookPopup"])
            }
            if(json["OpenUrl"]!==undefined){
                cppCef.openUrl(json["OpenUrl"])
            }
            if(json["Reload"]!==undefined){
                cppCef.reload(json["Reload"])
            }
            if(json["Load"]!==undefined){
                cppCef.load(json["Load"],json["Url"])
                callback(json["Load"],createSuccessMsg(str.id,request,cppCef.getCefClients(json["Load"])))
            }
            if(json["SetConfig"]!==undefined){
                cppConfig.set(json["SetConfig"],json["Value"])
            }
            if(json["GetConfig"]!==undefined){
                callback(json["GetConfig"],createSuccessMsg(str.id,request,cppConfig.get(json["Key"])))
            }
            if(json["Resize"]!==undefined){
                cppCef.resizeWindow(json["Resize"],json["Width"],json["Height"])
            }
            if(json["SetSystemTray"]!==undefined){
                cppCef.setTrayIcon(true)
                cppCef.setWindowHidden(true)
            }
            if(json["GetUUID"]!==undefined){
                callback(json["GetUUID"],createSuccessMsg(str.id,request,cppTool.getCpuID()+"|"+cppTool.getHostMacAddress()))
            }
            if(json["UpdateApp"]!==undefined){
                cppTool.startProcess(cppDir.downloadPath()+"/"+json["UpdateApp"]+".exe")
                cppTool.quitApp()
            }
            if(json["GetVersion"]!==undefined){
                callback(json["GetVersion"],createSuccessMsg(str.id,request,cppConfig.getVersion()))
            }
            if(json["GetAllData"]!==undefined){
                var data={"Cpuid":cppTool.getCpuID(),"Mac":cppTool.getHostMacAddress(),"MainUrl":cppConfig.getMainUrl(),"Version":cppConfig.getVersion(),"InstallPackName":cppConfig.getInstallPackName()}
                callback(json["GetAllData"],createSuccessMsg(str.id,request,JSON.stringify(data)))
            }
            if(json["OpenVideo"]!==undefined){
                cppCef.openVideo(json["RECT"])
            }
        }
    }
}


