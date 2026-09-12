import QtQuick
import QtQuick.Controls

Rectangle{
    SplitView.preferredHeight: parent.height * 0.2
    SplitView.minimumHeight: parent.height * 0.1
    SplitView.maximumHeight: parent.height * 0.3

    color: Constants.background
    ListView {
        id: filmStrip

        anchors.left: parent.left
        anchors.right: parent.right
        height: parent.height

        orientation: ListView.Horizontal
        spacing: 8

        model: appController.imageCollection
        currentIndex: appController.currentImageIndex

        property bool shouldAutoPosition: false

        Timer {
            id: autoPositionTimer
            interval: 50
            repeat: false

            onTriggered: filmStrip.positionCurrentImage()
        }
        onCurrentIndexChanged: {
            shouldAutoPosition = true
            autoPositionTimer.restart()
        }

        function positionCurrentImage() {
            if(filmStrip.shouldAutoPosition){
                const targetIndex = Math.max(
                    0,
                    appController.currentImageIndex - 3
                )

                positionViewAtIndex(
                    targetIndex,
                    ListView.Beginning
                )
                filmStrip.shouldAutoPosition = false;
            }
        }

        delegate: Column {
            spacing: 4

            Image {
                id: thumbnail

                height: filmStrip.height - 20
                width: implicitHeight > 0
                    ? implicitWidth * (height / implicitHeight)
                    : height

                source: model.thumbnail
                fillMode: Image.PreserveAspectFit

                onStatusChanged: {
                    if (status === Image.Ready) {
                        Qt.callLater(filmStrip.positionCurrentImage)
                    }
                }
            }

            Text {
                height: 20
                width: thumbnail.width
                text: model.fileName
                color: Constants.text

                horizontalAlignment: Text.AlignHCenter
                elide: Text.ElideRight
            }
        }
    }
}