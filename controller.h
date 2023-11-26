#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <vector>
#include <QtNetwork>


class Controller: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString udpData READ udpData WRITE setUdpData NOTIFY udpDataChanged)
    Q_PROPERTY(QString ip READ ip WRITE setIp NOTIFY ipChanged)
public:
    Controller();
    QString udpData() const;
    void setUdpData(const QString &newUdpData);

    QString ip() const;
    void setIp(const QString &newIp);
public Q_SLOTS:
    void startUDP();
    void writeDataEthernet(QString textToSend);
    void readDataEthernet();
Q_SIGNALS:
    void udpDataChanged();

    void sendChanged();

    void ipChanged();

private:
    QUdpSocket *udpSocketSend;
    QUdpSocket *udpSocketReceive;
    int receiverPort = 8569;
    int senderPort = 8570;
    QString m_udpData;
    QString m_ip = "192.168.1.3";
};

#endif // CONTROLLER_H
