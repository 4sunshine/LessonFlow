#ifndef ENGINEHANDLER_H
#define ENGINEHANDLER_H

#include <QQmlApplicationEngine>
#include <QQmlContext>

<<<<<<< HEAD:engine/enginehandler.h
#include "dataflow/sheetsmodel.h"
=======
#include "sheetsmodel.h"
>>>>>>> b83d31d... Visual behaviour improved:enginehandler.h

class EngineHandler : public QObject
{
    Q_OBJECT
public:
    explicit EngineHandler(QObject *parent = nullptr);

    void getSize(int width, int height);
    void getScreen();

signals:
    void subjectSelected(int, int);

public slots:
    void setQProperty(QString name, QStringList value);
    void classSelected(int, int);
    void setContext();
    void setOptimalCellWidth(int nstudents); //SET OPTIMAL GRIDVIEW CELL WIDTH

private:
    SheetsModel model;
    QQmlApplicationEngine engine;
    QObject *window;
    QQmlContext *ctxt;
    int e_width;
    int e_height;
};

#endif // ENGINEHANDLER_H
