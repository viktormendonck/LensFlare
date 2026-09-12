#include <complex>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <qqmlcontext.h>
#include <QLoggingCategory>
#include <QMessageLogContext>
#include <QString>

#include <cstdio>

#include "AppController.h"
#include "ThumbnailProvider.h"


static QtMessageHandler previousMessageHandler = nullptr;

static void lensflareMessageHandler(
    QtMsgType type,
    const QMessageLogContext& context,
    const QString& message
)
{
    const bool isKnownBreezeDialogNoise =
        message.contains("QtQuick/Dialogs/quickimpl") ||
        message.contains("org/kde/breeze") ||
        message.contains("Binding loop detected for property \"implicitWidth\"");

    if (isKnownBreezeDialogNoise) {
        return;
    }

    if (previousMessageHandler) {
        previousMessageHandler(type, context, message);
    }
}

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    previousMessageHandler = qInstallMessageHandler(lensflareMessageHandler);

    ImageCollectionModel* imageCollectionModel = new ImageCollectionModel();
    ImageProvider* imageProvider = new ImageProvider();

    engine.addImageProvider(
        "lensflare",
        imageProvider
    );
    engine.addImageProvider(
    "lensflare-thumbnail",
        new ThumbnailProvider(imageCollectionModel)
    );


    AppController controller(*imageProvider,imageCollectionModel);

    engine.rootContext()->setContextProperty(
        "appController",
        &controller
    );

    engine.loadFromModule(
        "Lensflare",
        "Main"
    );

    //load first image for testing
    controller.OpenFile(QUrl::fromLocalFile("/home/lazage/Pictures/Temp/6.June 2026/13-06-2026/IMG_2381.CR2"));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
