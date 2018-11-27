import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

ColumnLayout{
    Layout.fillHeight: false;
    property string slotreq;
    property var slotinfocmd;
    signal done();
    property alias headertxt : header.infotxt;

    function parseInfo(data){
        //act_pid.text = data[1]["value"];
        swver.infomsg  = data[0]["value"][0] + ".";
        swver.infomsg += data[0]["value"][1] + ".";
        swver.infomsg += data[0]["value"][2];
        hwver.infomsg = data[0]["value"][3];
        flashloc.infomsg = data[0]["value"][4] === 1 ? "TOP" : "BOT";
        swdate.infomsg  = data[3]
    }

    InfoRow{
        id: header;
        bold: true;
    }

    InfoRow{
        id: swver;
        infotxt: "Software Version:"
    }

    InfoRow{
        id: hwver;
        infotxt: "Radio HW:"
    }

    InfoRow{
        id: flashloc;
        infotxt: "Software flash Location:"
    }

    InfoRow{
        id: swdate;
        infotxt: "Software creation date:"
    }
}
