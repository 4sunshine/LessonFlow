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

    QObject::connect(&model, SIGNAL(gotCount(int)),
                     this, SLOT(setOptimalCellWidth(int)));

}

void EngineHandler::setQProperty(QString name, QStringList value)
{
    window->setProperty(name.toLocal8Bit(), value);
}

void EngineHandler::classSelected(int classNum, int lessonNumber)
{
    emit subjectSelected(classNum, lessonNumber);
}

void EngineHandler::setContext()
{
    ctxt -> setContextProperty("studflowModel", &model.studentsflow);
}

void EngineHandler::setOptimalCellWidth(int n) // N MEANS COUNT OF STUDENTS, X - SCREEN RATIO
{
    double x = double(e_width) / double(e_height);
    int n_w = int((1.+std::sqrt(1. + 4.*double(n)/x))*x*0.5); //OPTIMAL NUMBER OF WIDTH CELLS

/*
    n_w >= SQRT [ N * x ]
    n_w < 0.5x * [ 1 + SQRT( 1 + 4N/x ) ]
*/

    int cellWidth = e_width / n_w; //IF ALL SCREEN IS FILLED

// 0.95 EMPIRICAL COEFFICIENT

    window -> setProperty(QString("optWidth").toLocal8Bit(), 0.95 * cellWidth);
    window -> setProperty(QString("optCount").toLocal8Bit(), n_w);

    QObject::disconnect(&model, SIGNAL(gotCount(int)),
                        this, SLOT(setOptimalCellWidth(int))); //DISCONNECT SIGNAL AND SLOT
}

void EngineHandler::getSize(int width, int height)
{
    e_width = width;
    e_height = height;
}
