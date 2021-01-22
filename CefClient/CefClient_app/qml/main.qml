import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12

Item {
    id: root_father
    objectName: "root_father"
    visible: true
    width:100
    height:100

    Button{
        id:btn_call
        width: 100
        height: 100
        anchors.centerIn: parent.Center
        text: "call"
        background: Rectangle{
            color: "#EEEEEE"
        }

        onClicked: {
            var ret={"测试":"1111"}
            cppCef.sendToWeb("404","qtSlots("+JSON.stringify(ret)+")")
        }
    }

    Connections{
        target:cppCef
        property var taskMap:{0:0}
        onSgOpenNewWindow:function(father,jsonmsg){
            var json=JSON.parse(jsonmsg)
            var component;
            if(father===null){
                component=itemCompont.createObject(root_father,json)
            }else{
                component=itemCompont.createObject(father,json)
            }

            taskMap[json["objectName"]]=component
        }
        onSgCloseWindow:function(name){
            taskMap[name].destroy()
        }
    }


    Component {
        id:itemCompont
        Window{
            id:m_window
            visible:true
            Rectangle{
                anchors.fill: parent
                border.color: "red"
                border.width: 3
            }
        }
    }

    WebView{
        id: m_webview
        Connections {
            target: cppCef
            onSgWebMsgReceived:function(msg) {
                var str = JSON.parse(msg)
                if (str["msgName"] === "callQML") {
                    m_webview.jsobject.callQML(msg)
                }
            }
        }
    }
}
