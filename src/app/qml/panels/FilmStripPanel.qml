import QtQuick
import QtQuick.Controls

Rectangle{
    SplitView.preferredHeight: parent.height * 0.2
    SplitView.minimumHeight: parent.height * 0.1
    SplitView.maximumHeight: parent.height * 0.3

    color: Constants.background
    Label {
        anchors.centerIn: parent
        opacity: Constants.commentTextOpacity
        text: "Filmstrip"
    }
}