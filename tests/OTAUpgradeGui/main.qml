import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import Qt.labs.settings 1.0

Window {
    id: mainwindow;
    visible: true
    width: 860
    height: 290
    title: qsTr("Sensors Unleashed Firmware Upgrade Tool")
    color: "grey"

    property int upgradeSlot: 0;

    Loader{
        id: popoverloader;
        anchors.fill: parent;
        z: 10;

        onSourceChanged: if(source == "") activenode.request_SlotNfo(1);
    }

    ColumnLayout{
        anchors.fill: parent;
        anchors.margins: 10;
        spacing: 10

        UpgradeStartPage{
            width: parent.width;
            height: 100;
            //Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
        }

        RowLayout{
            ConnectingPage{
                id: active;
                headertxt: "Active slot"
                width: 300;
                height: 100;
                //Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
            }
            ConnectingPage{
                id: backup;
                headertxt: "Backup slot"
                width: 300;
                height: 100;
                //Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
            }
        }
    }

    Component.onCompleted: {
        //popoverloader.source = "Progress.qml";
        activenode.request_active_slot();
    }

    Connections{
        target: activenode;
        onRequst_received:{
            if(req === "req_slotNfoActive"){
                active.parseInfo(result);
                activenode.request_SlotNfo(0);
            }
            else if(req === "req_slotNfoBackup"){
                backup.parseInfo(result);
            }

            else if(req === "req_activeslot"){
                if(result[0]["value"] === 0){
                    upgradeSlot = 1;
                }
                else if(result[0]["value"] === 1){
                    upgradeSlot = 0;
                }
                activenode.request_SlotNfo(1);
            }
        }
    }
}
