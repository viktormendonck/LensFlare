#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <qqmlcontext.h>

#include "AppController.h"

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    AppController controller;
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("appController", &controller);
    engine.loadFromModule("Lensflare", "Main");


    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
