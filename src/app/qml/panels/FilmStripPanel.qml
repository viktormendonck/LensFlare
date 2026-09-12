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

        delegate: Rectangle {
            width: 140
            height: filmStrip.height

            border.width: 1
            radius: 4

            Text {
                anchors.centerIn: parent
                text: model.fileName
                elide: Text.ElideRight
                width: parent.width - 16
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }
}