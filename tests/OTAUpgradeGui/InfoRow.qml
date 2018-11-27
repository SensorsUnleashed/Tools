import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

RowLayout{

    property string infotxt: "NA";
    property string infomsg: "";
    property bool bold: false;
    Label{
        text: infotxt;
        font.pointSize: 10;
        font.bold: bold;
    }
    Label{
        text: infomsg;
        font.pointSize: 10;
    }
}
