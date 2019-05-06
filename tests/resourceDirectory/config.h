#ifndef CONFIG_H
#define CONFIG_H
#include <QHostAddress>

struct obsdevice{
    QHostAddress addr;
    quint16 port;
};

class config
{
public:
    config();

    void removeObserver(QHostAddress addr, quint16 port);
    void addObserver(QHostAddress addr, quint16 port);

    void getObserversList(QVector<obsdevice *> *list);

private:
    bool opened = false;
};

#endif // CONFIG_H
