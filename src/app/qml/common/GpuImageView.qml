import QtQuick

Item {
    id: root

    property url source
    property real exposure: 0.0

    implicitWidth: sourceImage.implicitWidth
    implicitHeight: sourceImage.implicitHeight

    Image {
        id: sourceImage

        anchors.fill: parent

        source: root.source
        asynchronous: false
        cache: false

        visible: false
    }

    ShaderEffect {
        anchors.fill: parent

        property var source: sourceImage
        property real exposure: root.exposure

        fragmentShader: "qrc:/shaders/ImagePreview.frag.qsb"
    }
}