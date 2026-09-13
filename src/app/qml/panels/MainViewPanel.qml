import QtQuick
import QtQuick.Controls

Rectangle {
    id: mainViewPanel

    SplitView.minimumWidth: parent.width * 0.55
    SplitView.fillWidth: true
    color: Constants.background

    property real zoomStep: 1.15
    property real fitPadding: 40
    property real minimumZoom: 0.05
    property real maximumZoom: 16.0
    property int controlPanelHeight: 35

    function zoomIn() {
        viewport.zoomAt(
            viewport.width / 2,
            viewport.height / 2,
            viewport.zoom * zoomStep
        )
    }

    function zoomOut() {
        viewport.zoomAt(
            viewport.width / 2,
            viewport.height / 2,
            viewport.zoom / zoomStep
        )
    }

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

            contentWidth: width + imageContainer.width
            contentHeight: height + imageContainer.height

            function clamp(value, minimum, maximum) {
                return Math.max(minimum, Math.min(maximum, value))
            }

            function centerImage() {
                contentX = imageContainer.width / 2
                contentY = imageContainer.height / 2
            }

            function fitZoom() {
                if (imageContainer.sourceWidth <= 0 ||
                    imageContainer.sourceHeight <= 0)
                    return

                const availableWidth = Math.max(
                    1,
                    width - mainViewPanel.fitPadding
                )

                const availableHeight = Math.max(
                    1,
                    height - mainViewPanel.fitPadding
                )

                const xScale =
                    availableWidth / imageContainer.sourceWidth

                const yScale =
                    availableHeight / imageContainer.sourceHeight

                zoom = Math.min(
                    xScale,
                    yScale,
                    mainViewPanel.maximumZoom
                )

                Qt.callLater(centerImage)
            }

            function zoomAt(mouseX, mouseY, requestedZoom) {
                if (imageContainer.sourceWidth <= 0 ||
                    imageContainer.sourceHeight <= 0)
                    return

                const oldWidth = imageContainer.width
                const oldHeight = imageContainer.height

                if (oldWidth <= 0 || oldHeight <= 0)
                    return

                const imageU =
                    (contentX + mouseX - imageContainer.x) / oldWidth

                const imageV =
                    (contentY + mouseY - imageContainer.y) / oldHeight

                const newZoom = clamp(
                    requestedZoom,
                    mainViewPanel.minimumZoom,
                    mainViewPanel.maximumZoom
                )

                if (newZoom === zoom)
                    return

                const newWidth =
                    imageContainer.sourceWidth * newZoom

                const newHeight =
                    imageContainer.sourceHeight * newZoom

                zoom = newZoom

                contentX = clamp(
                    imageContainer.x + imageU * newWidth - mouseX,
                    0,
                    Math.max(0, contentWidth - width)
                )

                contentY = clamp(
                    imageContainer.y + imageV * newHeight - mouseY,
                    0,
                    Math.max(0, contentHeight - height)
                )
            }

            Item {
                id: contentCanvas

                width: viewport.contentWidth
                height: viewport.contentHeight

                Item {
                    id: imageContainer

                    x: viewport.width / 2
                    y: viewport.height / 2

                    readonly property real sourceWidth:
                        appController.previewReady
                            ? gpuImage.implicitWidth
                            : loadingThumbnail.implicitWidth

                    readonly property real sourceHeight:
                        appController.previewReady
                            ? gpuImage.implicitHeight
                            : loadingThumbnail.implicitHeight

                    width: sourceWidth * viewport.zoom
                    height: sourceHeight * viewport.zoom

                    Image {
                        id: loadingThumbnail

                        anchors.fill: parent

                        source: appController.loadingThumbnailSource
                        fillMode: Image.PreserveAspectFit

                        visible: !appController.previewReady
                        asynchronous: false
                        cache: true

                        onStatusChanged: {
                            if (status === Image.Ready &&
                                !appController.previewReady)
                            {
                                Qt.callLater(viewport.fitZoom)
                            }
                        }
                    }

                    GpuImageView {
                        id: gpuImage

                        anchors.fill: parent

                        source: appController.previewReady
                            ? appController.previewSource
                            : ""

                        exposure: appController.exposure

                        visible: appController.previewReady

                        onImplicitWidthChanged: {
                            if (appController.previewReady &&
                                implicitWidth > 0 &&
                                implicitHeight > 0)
                            {
                                Qt.callLater(viewport.fitZoom)
                            }
                        }

                        onImplicitHeightChanged: {
                            if (appController.previewReady &&
                                implicitWidth > 0 &&
                                implicitHeight > 0)
                            {
                                Qt.callLater(viewport.fitZoom)
                            }
                        }
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
            acceptedButtons: appController.previewReady
                ? Qt.LeftButton
                : Qt.NoButton

            hoverEnabled: appController.previewReady
            preventStealing: true
            enabled: appController.previewReady

            property real dragStartX: 0
            property real dragStartY: 0
            property real contentStartX: 0
            property real contentStartY: 0

            onWheel: wheel => {
                if (!appController.previewReady) {
                    wheel.accepted = true
                    return
                }

                const factor =
                    wheel.angleDelta.y > 0
                        ? mainViewPanel.zoomStep
                        : 1.0 / mainViewPanel.zoomStep

                viewport.zoomAt(
                    wheel.x,
                    wheel.y,
                    viewport.zoom * factor
                )

                wheel.accepted = true
            }

            onPressed: mouse => {
                if (!appController.previewReady)
                    return

                dragStartX = mouse.x
                dragStartY = mouse.y
                contentStartX = viewport.contentX
                contentStartY = viewport.contentY
            }

            onPositionChanged: mouse => {
                if (!appController.previewReady)
                    return

                if (!(mouse.buttons & Qt.LeftButton))
                    return

                const deltaX = mouse.x - dragStartX
                const deltaY = mouse.y - dragStartY

                viewport.contentX = viewport.clamp(
                    contentStartX - deltaX,
                    0,
                    Math.max(
                        0,
                        viewport.contentWidth - viewport.width
                    )
                )

                viewport.contentY = viewport.clamp(
                    contentStartY - deltaY,
                    0,
                    Math.max(
                        0,
                        viewport.contentHeight - viewport.height
                    )
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
        height: mainViewPanel.controlPanelHeight

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
                    viewport.zoom / mainViewPanel.zoomStep
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
                    viewport.zoom * mainViewPanel.zoomStep
                )
            }

            LFButton {
                text: "Fit"
                onClicked: viewport.fitZoom()
            }
        }
    }
}