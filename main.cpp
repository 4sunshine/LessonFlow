#include <QGuiApplication>
#include <QScreen>

#include "engine/enginehandler.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QScreen * screen = app.primaryScreen(); //GET SIZE OF SCREEN

    QRect screenRect = screen->geometry(); //GET SIZE OF SCREEN

    EngineHandler engineHandler; //TO HANDLE QML PART OF APPLICATION

    engineHandler.getSize(screenRect.width(),screenRect.height()); //GET SIZE OF SCREEN

    return app.exec();
}
