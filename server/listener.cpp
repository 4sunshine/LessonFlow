#include "listener.h"


Listener::Listener(QObject *parent) : QObject(parent)
{
    server = new QTcpServer(this);
    connect(server, &QTcpServer::newConnection, this, &Listener::newConnection);
    server->listen(QHostAddress::Any,9090);
/*    if(!server->listen(QHostAddress::Any, 9090)){
        qInfo() << "server is not started";
    } else {
        qInfo() << "server is started";
    }
*/
}

void Listener::newConnection()
{
    socket = server->nextPendingConnection();
    emit connected();

//    socket -> write("Hello, World!!! I am echo server!\r\n");

    connect(socket, &QTcpSocket::readyRead, this, &Listener::readMessage);
    connect(socket, &QTcpSocket::disconnected, this, &Listener::disconnected);
}

void Listener::readMessage()
{
    while((socket->bytesAvailable()>0) && isActive)
    {
        QByteArray array = socket->readAll();
        qInfo() << array;
        if (array.contains(";"))
        {
            QList<QByteArray> recData;//RECEIVED DATA
            recData = array.split(";"[0]);
            for (int i = 0; i < recData.length(); ++i) {
                emit buttonEvent(recData[2*i].toInt());//READ BUTTON ID
                qInfo() << "BUTTON EVENT";
                qInfo() << recData[2*i];
                qInfo() << recData[2*i+1];
            }
        }
        else {
            //CONVERT BYTES TO DEC CODE OF BUTTON
            if (array.toInt(nullptr,16)){
                emit irSignal(array.toInt(nullptr,16));
            }
        }
    }
}

void Listener::disconnected()
{
    socket->close();
}
