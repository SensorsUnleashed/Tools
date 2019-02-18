import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

Window {
    id: mainwindow;
    visible: true
    width: 140
    height: 200
    title: qsTr("Sensors Unleashed Firmware Upgrade Tool")
    color: "grey"

    property int fontsize: 48

    ColumnLayout{
        anchors.fill: parent;
        Layout.fillHeight : true;
        Layout.fillWidth : true;

        RowLayout{
            spacing: 10;
            Text{
                Layout.fillWidth: true;
                text: kettle.tempKettle.toFixed(1);
                font.pointSize: fontsize;
                Layout.alignment: Qt.AlignRight;
                horizontalAlignment: Text.AlignRight;
                rightPadding: 10;
            }
            Label{
                Layout.preferredWidth: 50;
                text: "ºC"
                font.pointSize: fontsize;
            }
        }
        RowLayout{
            Text{
                Layout.fillWidth: true;
                text: kettle.power.toFixed(1);
                font.pointSize: fontsize;
                horizontalAlignment: Text.AlignRight;
                rightPadding: 10;
            }
            Label{
                Layout.preferredWidth: 50;
                text: "kW"
                font.pointSize: fontsize;
            }
        }
        RowLayout{
        ProgressBar {
            Layout.fillWidth: true;
            Layout.fillHeight: false;
            value: kettle.dutyCycle;
            to: 100.0;
            from: 0.0;
        }
        Text{
            Layout.fillWidth: false;
            Layout.fillHeight: false;
            Layout.preferredWidth: 15;
            text: kettle.dutyCycle;
            horizontalAlignment: Text.AlignRight;
        }
        Text{
            Layout.fillWidth: false;
            Layout.fillHeight: false;
            Layout.preferredWidth: 15;
            text: "%";
        }
        }
        RowLayout{
            ProgressBar {
                Layout.fillWidth: true;
                Layout.fillHeight: false;
                value: 160 - kettle.consumption;
                to: 160.0;
                from: 0.0;
            }
            Text{
                Layout.fillWidth: false;
                Layout.fillHeight: false;
                Layout.preferredWidth: 15;
                text: (kettle.consumption).toFixed(1);
                horizontalAlignment: Text.AlignRight;
            }
            Text{
                Layout.fillWidth: false;
                Layout.fillHeight: false;
                Layout.preferredWidth: 15;
                text: "kg";
            }
        }
        RowLayout{
            Text{
                text: "Driftstatus:"
            }
            Text{
                text: kettle.state;
            }
        }
        RowLayout{
            Text{
                text: "Alarmstatus:"
            }
            Text{
                text: kettle.alarm;
            }
        }


    }
}
