import QtQuick
import QtQuick.Controls.Basic

Rectangle{
    id: root

    readonly property int imageCount: filmStrip.count

    function centerCurrentImage() {
        filmStrip.shouldAutoPosition = true
        filmStrip.positionCurrentImage()
    }

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

        ScrollBar.horizontal: ScrollBar {
            id: filmStripScrollBar

            policy: ScrollBar.AlwaysOn
            implicitHeight: 10

            background: Rectangle {
                implicitHeight: 10
                color: Constants.accentBackground
            }

            contentItem: Rectangle {
                implicitHeight: 10
                radius: height / 2

                color: filmStripScrollBar.pressed
                    ? Constants.selectedOutlineColor
                    : filmStripScrollBar.hovered
                        ? Constants.selectedColor
                        : Constants.spacer
            }
        }

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

        delegate: Rectangle {
            width: thumbnailColumn.width + 8
            height: thumbnailColumn.height + 8

            radius: 4

            color: {
                if (index === appController.currentImageIndex)
                    return Constants.selectedColor

                if (mouseArea.containsMouse)
                    return Constants.selectedHoverColor

                return "transparent"
            }

            border.width: index === appController.currentImageIndex ? 1 : 0
            border.color: Constants.selectedOutlineColor


            Column {
                id: thumbnailColumn

                anchors.centerIn: parent
                spacing: 4

                Image {
                    id: thumbnail

                    height: filmStrip.height - 43
                    width: height * model.aspectRatio

                    source: model.thumbnail
                    fillMode: Image.PreserveAspectFit
                }

                Text {
                    width: thumbnail.width
                    height: 20

                    text: model.fileName
                    color: Constants.text

                    horizontalAlignment: Text.AlignHCenter
                    elide: Text.ElideRight
                }

            }
            MouseArea {
                id: mouseArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: appController.OpenImage(index)
            }
        }
    }
}