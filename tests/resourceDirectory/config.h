#ifndef CONFIG_H
#define CONFIG_H
#include <QHostAddress>

struct obsdevice{
    QHostAddress addr;
    quint16 port;
};

struct nodeinfo{
    int nodeid;
    QHostAddress addr;
    quint16 port;
    QString location;
};

struct deviceinfo{
    int deviceid;
    int nodeid;
    QByteArray uri;
    QString attributes;
};

class config
{
public:
    config();

    void removeObserver(QHostAddress addr, quint16 port);
    void addObserver(QHostAddress addr, quint16 port);

    void getObserversList(QVector<obsdevice *> *list);

    void getNodes(QVector<nodeinfo*> *list);

    void getDevices(int nodeid, QVector<deviceinfo*> *list);
    int addDevice(int nodeid, QByteArray uri, int devicetype);

    void insertNewData(int deviceid, QString value);

private:
    bool opened = false;
};

#endif // CONFIG_H
