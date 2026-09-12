import QtQuick
import QtQuick.Controls

Rectangle {
    id: mainViewPanel

    SplitView.minimumWidth: parent.width * 0.55
    SplitView.fillWidth: true
    color: Constants.background

    Item {
        id: viewportContainer

        anchors.top: parent.top
        anchors.bottom: controlPanel.top
        anchors.left: parent.left
        anchors.right: parent.right

        Flickable {
            id: viewport

            anchors.fill: parent
            clip: true
            interactive: false
            boundsBehavior: Flickable.StopAtBounds

            property real zoom: 1.0
            property real minimumZoom: 0.05
            property real maximumZoom: 16.0

            contentWidth: viewport.width + image.width
            contentHeight: viewport.height + image.height

            function clamp(value, minimum, maximum) {
                return Math.max(minimum, Math.min(maximum, value))
            }

            function centerImage() {
                contentX = image.width / 2
                contentY = image.height / 2
            }

            function fitZoom() {
                if (image.implicitWidth <= 0 || image.implicitHeight <= 0)
                    return

                const padding = 40
                const availableWidth = Math.max(1, width - padding)
                const availableHeight = Math.max(1, height - padding)
                const xScale = availableWidth / image.implicitWidth
                const yScale = availableHeight / image.implicitHeight

                zoom = Math.min(xScale, yScale)
                centerImage()
            }

            function zoomAt(mouseX, mouseY, requestedZoom) {
                if (image.implicitWidth <= 0 || image.implicitHeight <= 0)
                    return

                const oldWidth = image.width
                const oldHeight = image.height
                const oldImageX = viewport.width / 2
                const oldImageY = viewport.height / 2
                const imageU = (contentX + mouseX - oldImageX) / oldWidth
                const imageV = (contentY + mouseY - oldImageY) / oldHeight
                const newZoom = clamp(requestedZoom, minimumZoom, maximumZoom)

                if (newZoom === zoom)
                    return

                const newImageWidth = image.implicitWidth * newZoom
                const newImageHeight = image.implicitHeight * newZoom
                const newImageX = viewport.width / 2
                const newImageY = viewport.height / 2

                zoom = newZoom

                contentX = clamp(
                    newImageX + imageU * newImageWidth - mouseX,
                    0,
                    Math.max(0, contentWidth - width)
                )

                contentY = clamp(
                    newImageY + imageV * newImageHeight - mouseY,
                    0,
                    Math.max(0, contentHeight - height)
                )
            }

            Item {
                id: contentCanvas

                width: viewport.contentWidth
                height: viewport.contentHeight

                Image {
                    id: image

                    x: viewport.width / 2
                    y: viewport.height / 2
                    width: implicitWidth * viewport.zoom
                    height: implicitHeight * viewport.zoom

                    source: appController.imageRevision > 0
                        ? "image://lensflare/current?v=" + appController.imageRevision
                        : ""

                    asynchronous: false
                    cache: false

                    onStatusChanged: {
                        if (status === Image.Ready)
                            viewport.fitZoom()
                    }
                }
            }

            ScrollBar.horizontal: ScrollBar {
                policy: ScrollBar.AsNeeded
            }

            ScrollBar.vertical: ScrollBar {
                policy: ScrollBar.AsNeeded
            }
        }

        MouseArea {
            id: inputArea

            anchors.fill: parent
            acceptedButtons: Qt.LeftButton
            hoverEnabled: true
            preventStealing: true

            property real dragStartX: 0
            property real dragStartY: 0
            property real contentStartX: 0
            property real contentStartY: 0

            onWheel: wheel => {
                const factor = wheel.angleDelta.y > 0 ? 1.15 : 1.0 / 1.15

                viewport.zoomAt(
                    wheel.x,
                    wheel.y,
                    viewport.zoom * factor
                )

                wheel.accepted = true
            }

            onPressed: mouse => {
                dragStartX = mouse.x
                dragStartY = mouse.y
                contentStartX = viewport.contentX
                contentStartY = viewport.contentY
            }

            onPositionChanged: mouse => {
                if (!(mouse.buttons & Qt.LeftButton))
                    return

                const deltaX = mouse.x - dragStartX
                const deltaY = mouse.y - dragStartY

                viewport.contentX = viewport.clamp(
                    contentStartX - deltaX,
                    0,
                    Math.max(0, viewport.contentWidth - viewport.width)
                )

                viewport.contentY = viewport.clamp(
                    contentStartY - deltaY,
                    0,
                    Math.max(0, viewport.contentHeight - viewport.height)
                )
            }
        }

        PinchHandler {
            id: pinchHandler

            target: null

            property real startingZoom: 1.0

            onActiveChanged: {
                if (active)
                    startingZoom = viewport.zoom
            }

            onActiveScaleChanged: {
                if (!active)
                    return

                viewport.zoomAt(
                    centroid.position.x,
                    centroid.position.y,
                    startingZoom * activeScale
                )
            }
        }
    }

    Rectangle {
        id: controlPanel

        anchors.right: parent.right
        anchors.bottom: parent.bottom
        width: parent.width
        height: 35
        color: Constants.accentBackground

        Row {
            anchors.left: parent.left
            anchors.leftMargin: 8
            anchors.verticalCenter: parent.verticalCenter
            spacing: 6

            LFButton {
                text: "-"
                onClicked: viewport.zoomAt(
                    viewport.width / 2,
                    viewport.height / 2,
                    viewport.zoom / 1.15
                )
            }

            Label {
                width: 50
                anchors.verticalCenter: parent.verticalCenter
                horizontalAlignment: Text.AlignHCenter
                opacity: Constants.commentTextOpacity
                text: Math.round(viewport.zoom * 100) + "%"
            }

            LFButton {
                text: "+"
                onClicked: viewport.zoomAt(
                    viewport.width / 2,
                    viewport.height / 2,
                    viewport.zoom * 1.15
                )
            }

            LFButton {
                text: "Fit"
                onClicked: viewport.fitZoom()
            }
        }
    }
}