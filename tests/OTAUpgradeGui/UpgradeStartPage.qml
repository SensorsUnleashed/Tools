import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import Qt.labs.settings 1.0


ColumnLayout{
    id: upgradewidget;
    Layout.fillHeight: false;
    spacing: 15;
    property string filefilter: upgradeSlot == 1 ? "Binary files (*.fltop)" : "Binary files (*.flbot)"
    property string filepath;

    RowLayout{
        Layout.fillHeight: false;
        Rectangle{
            id: commstatus;
            width: 40;
            height: 40;
            radius: width / 2;
            color: activenode.commStatus == 0 ? "Green" : activenode.commStatus == 1 ? "red" : "lightgrey";
            border.width: 1;

            MouseArea{
                anchors.fill: parent;
                onPressed: commstatus.opacity = 0.80;
                onReleased: commstatus.opacity = 1;
                onClicked: {
                    activenode.request_active_slot();
                    blinktimer.start();
                }
            }

            Timer {
                id: blinktimer;
                interval: 500; running: false; repeat: true
                onTriggered: {
                    commstatus.opacity = commstatus.opacity == 1 ? 0.5 : 1;
                }
            }

            Connections{
                target: activenode

                onCommStatusChanged: {
                    commstatus.opacity = 1;
                    blinktimer.stop();
                }
            }
        }

        Rectangle{
            Layout.fillWidth: true
            Layout.preferredHeight: commstatus.height;
            border.width: 1;
            color: "White"
            Label{
                text: activenode.getAddressStr();
                padding: 10;
                font.pointSize: 18;
                anchors.centerIn: parent;
            }
        }
    }
    RowLayout{
        Layout.fillHeight: false;
        Button{
            text: "File";
            onClicked: swfilepickerloader.sourceComponent = swfilepicker;
        }
        Rectangle{
            Layout.fillWidth: true;
            height: 40;
            color: "white";
            border.width: 1;
            border.color: "black"
            TextInput{
                id: filenamefield;
                height: parent.height;
                verticalAlignment: TextInput.AlignVCenter
                leftPadding: 5;
                text: ""
            }
        }
    }
    RowLayout{
        Layout.fillHeight: false;
        Button{
            text: "Reset";
            onClicked: activenode.request_swreset();
        }
        Button{
            Layout.fillWidth: true;
            text: "Upgrade";
            enabled: filenamefield.text !== "" ? true : false;
            onClicked: {
                var t = activenode.swupgrade(filenamefield.text);
                popoverloader.setSource("Progress.qml",{"token": t});
            }

        }
    }

    Settings{
        id: settings;
        property alias filepath: upgradewidget.filepath;
    }

    Loader{
        id: swfilepickerloader;
    }

    Component{
        id: swfilepicker;

        FileDialog {
            id: fileDialog
            title: "Please choose a file"
            folder: filepath;
            nameFilters: [ filefilter ]

            onAccepted: {
                filenamefield.text = fileUrl;
                swfilepickerloader.sourceComponent = null;
            }
            onRejected: {
                swfilepickerloader.sourceComponent = null;
            }
            Component.onCompleted: {
                console.log("filepath: " + folder + " filefilter: " + filefilter);
                visible = true
            }
            Component.onDestruction: {
                filepath = folder;
            }
        }
    }
}


