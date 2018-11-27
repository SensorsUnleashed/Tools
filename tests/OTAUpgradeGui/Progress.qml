import QtQuick 2.11
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

Rectangle {
    property int token : 0;
    anchors.fill: parent;
    color: "lightgrey";
    //To avoid clicking the background
    MouseArea{
        anchors.fill: parent;

    }

    ColumnLayout{
        width: parent.width * 0.9;
        anchors.centerIn: parent;

        Label{
            id: progressinfo;
            text: "0/0";
            font.pointSize: 10;
        }

        ProgressBar{
            id: bar;
            height: 40;
            minimumValue: 0;
            Layout.fillWidth: true;
        }

        RowLayout{
            Button {
                id: close;
                text: "Close";
                visible: false;
                onClicked: {
                    popoverloader.source = "";
                }
            }
            Button {
                id: abort;
                text: "Abort";

                onClicked: {
                    activenode.abort(token);
                    visible = false;
                    close.visible = true;
                }
            }
            Button {
                id: retry;
                enabled: false;
                text: "Retry";
                onClicked: {
                    activenode.retry(token);
                    enabled: false;
                }
            }

        }
    }

    Connections{
        target: activenode;
        onTx_progress:{
            bar.maximumValue = totallen;
            bar.value = byteindex;
            progressinfo.text = byteindex + "/" + totallen;

            if(totallen == byteindex && byteindex > 0) {
                abort.visible = false;
                close.visible = true;
                retry.visible = false;
                progressinfo.text = "Done!";
            }
        }

        onTx_timeout:{
            retry.enabled = true;
        }
    }
}

