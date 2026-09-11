import QtQuick
import QtQuick.Controls
Rectangle {
    SplitView.preferredWidth: parent.width * 0.115
    SplitView.minimumWidth: parent.width * 0.05
    color: Constants.background

    Rectangle{
        anchors.right: parent.right
        anchors.top: parent.top
        width: parent.width
        height: 20
        color: Constants.accentBackground
        Label {
            anchors.centerIn: parent
            opacity: Constants.commentTextOpacity
            text: "Folder View"
        }
    }
}