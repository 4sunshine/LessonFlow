#include "enginehandler.h"
#include <QtCore/QDebug>

EngineHandler::EngineHandler(QObject *parent) : QObject(parent),
    model()
{
    QObject::connect(&model,SIGNAL(dataGot(QString, QStringList)),
                    this,SLOT(setQProperty(QString, QStringList)));

    QObject::connect(this,SIGNAL(subjectSelected(int,int)),
                         &model,SLOT(classSelected(int,int)));

    QObject::connect(&model,SIGNAL(gridPrepared()),
                     this,SLOT(setContext()));

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    window = engine.rootObjects()[0];
    ctxt = engine.rootContext();
    QObject::connect(window,SIGNAL(subjectSelected(int, int)),
                     this,SLOT(classSelected(int, int)));

}

void EngineHandler::setQProperty(QString name, QStringList value)
{
    window->setProperty(name.toLocal8Bit(), value);
    if (name == "popups")
    {
        qDebug()<<"POPUPS C++";
    }
}

void EngineHandler::classSelected(int classNum, int lessonNumber)
{
    emit subjectSelected(classNum, lessonNumber);
}

void EngineHandler::setContext()
{
    ctxt -> setContextProperty("studflowModel", &model.studentsflow);
    qDebug()<<"CONTEXT MUST BE CHANGED";
}

void EngineHandler::changeWindowSize()//LOOK AT THAT
{
    window->setProperty(QString("height").toLocal8Bit(),200);
}
