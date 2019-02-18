#ifndef WOODY_H
#define WOODY_H

#include <QString>
#include <QVariantMap>

class woody : public QObject
{

    Q_OBJECT
    Q_PROPERTY(QVariant power READ power NOTIFY operatingDataChanged)
    Q_PROPERTY(QVariant dutyCycle READ dutyCycle NOTIFY operatingDataChanged)
    Q_PROPERTY(QVariant tempKettle READ tempKettle NOTIFY operatingDataChanged)
    Q_PROPERTY(QVariant consumption READ consumption NOTIFY operatingDataChanged)
    Q_PROPERTY(QVariant state READ state NOTIFY operatingDataChanged)
    Q_PROPERTY(QVariant alarm READ alarm NOTIFY operatingDataChanged)

public:
    enum e_status {
        vent = 0,

        optaend_1 = 2,

        optaend_2 = 4,
        drift = 5,
        pause_ = 6,

        lav_kedeltemperatur_fejl = 8,
        stoppet = 9,

        termofoeler_varm = 11,
        stik_afmonteret = 12,
        optaending_fejl = 13,
        slukket = 14,
        kedelfoeler_fejl = 15,
        fotofoeler_fejl = 16,
        skaktfoeler_fejl = 17
    };

    enum e_alarm {
            Ok,
            LowBoilerTempErr,
            IgnitionErr,
            HighShaftTempErr,
            BoilerTempSensorErr,
            LightSensorErr,
            ShaftSensorErr,
            NoFireErr
    };

    woody();

    void setOperatingdata(QString opdata);

    Q_INVOKABLE QVariant reading(QString req);

    QVariant power() { return reading("power"); }
    QVariant dutyCycle() { return reading("dutyCycle").toDouble(); }
    QVariant tempKettle() { return reading("tempKettle"); }
    QVariant consumption() { return reading("consumption"); }
    QVariant state(){ return get_operatingstatus(reading("state").toInt()); }
    QVariant alarm(){ return get_alarm_string(reading("alarm").toInt()); }
private:
    QVariantMap data;

    QString get_operatingstatus(int status);
    QString get_alarm_string(int status);
signals:
    void operatingDataChanged();
};

#endif // WOODY_H
