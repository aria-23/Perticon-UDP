#include "controller.h"

Controller::Controller()
{

}

void Controller::startUDP()
{
    udpSocketReceive = new QUdpSocket(this);
    udpSocketReceive->bind(receiverPort, QUdpSocket::ShareAddress); //Allow other services to bind to the same address and port.
    bool success = connect(udpSocketReceive, SIGNAL(readyRead()), this, SLOT(readDataEthernet()));   //When a packet is received, call onPacketRecieved()
    qDebug() << "Check Assert Receive" << success;

    // Send Data
    udpSocketSend = new QUdpSocket(this); //create UDP socket to send to
    udpSocketSend->bind(senderPort,QUdpSocket::ShareAddress); //Allow other services to bind to the same address and port.
    //connect(this, SIGNAL(sendChanged()), this, SLOT(writeData()));
    qDebug() << "Check Assert Send" << success;
}

void Controller::readDataEthernet()
{
    QByteArray datagram;

    do{
        datagram.resize(udpSocketReceive->pendingDatagramSize());
        udpSocketReceive->readDatagram(datagram.data(), datagram.size());
        setUdpData(QString(datagram.data()));
    }
    while(udpSocketReceive->hasPendingDatagrams());

}

void Controller::writeDataEthernet(QString textToSend)
{
    QByteArray txDatagram;
    txDatagram = textToSend.toUtf8();
    udpSocketSend->writeDatagram(txDatagram ,QHostAddress(m_ip), senderPort);
}

QString Controller::udpData() const
{
    return m_udpData;
}

void Controller::setUdpData(const QString &newUdpData)
{
    if (m_udpData == newUdpData)
        return;
    m_udpData = newUdpData;
    Q_EMIT udpDataChanged();
}


QString Controller::ip() const
{
    return m_ip;
}

void Controller::setIp(const QString &newIp)
{
    if (m_ip == newIp)
        return;
    m_ip = newIp;
    Q_EMIT ipChanged();
}
