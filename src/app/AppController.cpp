#include "AppController.h"

#include "raw/RawDecoder.h"
#include <iostream>

#include "ImageProvider.h"


AppController::AppController(::ImageProvider& imageProvider)
    : AppImageProvider(imageProvider)
{
}

void AppController::OpenFile(const QUrl& url)
{
    const QString localPath = url.toLocalFile();

    if (localPath.isEmpty()) {
        SetStatusText("Invalid file path");
        return;
    }

    try {
        const Image image = AppRawDecoder.decode(
            std::filesystem::path(localPath.toStdString())
        );

        QImage qtImage(
            image.pixels.data(),
            image.width,
            image.height,
            image.width * image.channels,
            QImage::Format_RGB888
        );

        qtImage = qtImage.copy();
        AppImageProvider.setImage(std::move(qtImage));
        SetImageRevision(ImageRevision+1);
        SetStatusText(
            QString("Opened %1")
                .arg(localPath)
        );
    }
    catch (const std::exception& error) {
        SetStatusText(
            QString("Failed to open RAW: %1")
                .arg(error.what())
        );
    }
}


void AppController::SaveButton()
{
    SetStatusText("Save Pressed");
}

void AppController::UndoButton()
{
    SetStatusText("Undo Pressed");
}

void AppController::RedoButton()
{
    SetStatusText("Redo Pressed");
}
