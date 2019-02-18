#include "woody.h"

woody::woody()
{


}

void woody::setOperatingdata(QString opdata){
    int offset = 0;
    QStringRef ref(&opdata);
    data["dutyCycle"] = ref.mid(offset, 5).toDouble(); offset += 5;
    data["power"] = ref.mid(offset, 5).toDouble() / 10; offset += 5;
    data["tempKettle"] = ref.mid(offset, 5).toDouble() / 10; offset += 5;
    data["tempShaft"] = ref.mid(offset, 5).toDouble(); offset += 5;
    data["tempSmoke"] = ref.mid(offset, 5).toDouble(); offset += 5;
    data["oxygen"] = ref.mid(offset, 5).toDouble(); offset += 5;
    data["lightLevel"] = ref.mid(offset, 5).toDouble(); offset += 5;
    data["feederTimeSec"] = ref.mid(offset, 10).toInt(); offset += 10;
    data["ignitionTimeSec"] = ref.mid(offset, 10).toInt(); offset += 10;
    data["alarm"] = ref.mid(offset, 5).toDouble(); offset += 5;
    data["kettelSetpoint"] = ref.mid(offset, 5).toDouble(); offset += 5;
    data["oxygenSetpoint"] = ref.mid(offset, 5).toDouble(); offset += 5;
    data["capacity"] = ref.mid(offset, 5).toDouble(); offset += 5;
    data["chimneyType"] = ref.mid(offset, 5).toDouble(); offset += 5;
    data["satLow"] = ref.mid(offset, 5).toDouble(); offset += 5;
    data["satHigh"] = ref.mid(offset, 5).toDouble(); offset += 5;
    data["state"] = ref.mid(offset, 5).toInt(); offset += 5;
    data["type"] = ref.mid(offset, 5).toInt(); offset += 5;

    double consumption = data["capacity"].toDouble()/360.0*data["feederTimeSec"].toInt()/1000.0;
    double pellets_left = 200 - consumption;
    pellets_left = pellets_left > 0 ? pellets_left : 0;

    data["consumption"] = consumption;
    data["tanklevel"] = pellets_left;

    emit operatingDataChanged();
}

QVariant woody::reading(QString req){

    if(req.isEmpty()) return data;
    if(!data.contains(req)) return QString("");
    return data[req];
}

QString woody::get_operatingstatus(int status){

        QString str;

        switch(status){
        case vent:
                str = "Vent";
                break;
        case optaend_1:
                str = "Optænd 1";
                break;
        case optaend_2:
                str = "Optænd 2";
                break;
        case drift:
                str = "I drift";
                break;
        case pause_:
                str = "Pause";
                break;
        case lav_kedeltemperatur_fejl:
                str = "Lav kedeltemperatur fejl";
                break;
        case stoppet:
                str = "Stoppet";
                break;
        case termofoeler_varm:
                str = "Termoføler varm";
                break;
        case stik_afmonteret:
                str = "Stik afmonteret";
                break;
        case optaending_fejl:
                str = "Optændingsfejl";
                break;
        case slukket:
                str = "Slukket";
                break;
        case kedelfoeler_fejl:
                str = "Kedelføler fejl";
                break;
        case fotofoeler_fejl:
                str = "Fotoføler fejl";
                break;
        case skaktfoeler_fejl:
                str = "Skaktføler fejl";
                break;
        }

        return str;
}

QString woody::get_alarm_string(int status){

        QString str;

        switch(status){
        case Ok:
                str = "Ingen fejl";
                break;
        case LowBoilerTempErr:
                str = "Lav kedel temperatur";
                break;
        case IgnitionErr:
                str = "Optændings fejl";
                break;
        case HighShaftTempErr:
                str = "Høj skakt temperatur";
                break;
        case BoilerTempSensorErr:
                str = "Kedel temperatursensor fejl";
                break;
        case LightSensorErr:
                str = "Lyssensor fejl";
                break;
        case ShaftSensorErr:
                str = "Skakt temperatursensor fejl";
                break;
        case NoFireErr:
                str = "Ingen Ild kan detekteres";
                break;

        }
        return str;
}

