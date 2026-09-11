import QtQuick
import QtQuick.Controls
Rectangle {
    id: editingPanel
    SplitView.preferredWidth: parent.width * 0.15
    SplitView.minimumWidth: parent.width * 0.095
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
            text: "Editing panel"
        }
    }
}