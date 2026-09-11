import QtQuick
import QtQuick.Controls
Rectangle {
    id: mainViewPanel
    SplitView.minimumWidth: parent.width * 0.55
    SplitView.fillWidth: true
    color: Constants.background
    //yippie i have comments

    Label {
            anchors.centerIn: parent
            opacity: Constants.commentTextOpacity
            text: "Main View Panel"
    }

    Rectangle{
        id: controlPanel
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        width: parent.width
        color: Constants.accentBackground
        height: 35

        Label {
            anchors.centerIn: parent
            opacity: Constants.commentTextOpacity
            text: "control bar"
        }
    }

}
