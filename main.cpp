#include <QGuiApplication>
#include "enginehandler.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    EngineHandler engineHandler; //TO HANDLE QML PART OF APPLICATION

    return app.exec();
}
