#ifndef ENGINEHANDLER_H
#define ENGINEHANDLER_H

#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "sheetsmodel.h"

class EngineHandler : public QObject
{
    Q_OBJECT
public:
    explicit EngineHandler(QObject *parent = nullptr);


signals:
    void subjectSelected(int);

public slots:
    void setQProperty(QString name, QStringList value);
    void classSelected(int);
    void setContext();
    void changeWindowSize();

private:
    SheetsModel model;
    QQmlApplicationEngine engine;
    QObject *window;
    QQmlContext *ctxt;
};

#endif // ENGINEHANDLER_H
