#ifndef LISTENER_H
#define LISTENER_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QHostAddress>


class Listener : public QObject
{
    Q_OBJECT
public:
    explicit Listener(QObject *parent = nullptr);
    bool isActive = false; //isActiveState of server

signals:
    void buttonEvent(int buttonId); //STUDENT PUSHED BUTTON EVENT
    void irSignal(int irCode); //BUTTON SIGNAL WITH ID


public slots:
    void newConnection();
    void readMessage();
    void disconnected();

private:
    QTcpServer * server;
    QTcpSocket * socket;
    // FROM HEX TO DEC IR CODES
/*    enum {
        UP = 16736925,
        DOWN = 16754775,
        OK = 16712445,
        LEFT = 16720605,
        RIGHT = 16761405,
        TWO = 16750695,
        THREE = 16756815,
        FOUR = 16724175,
        FIVE = 16718055
    };
*/
};

#endif // LISTENER_H
