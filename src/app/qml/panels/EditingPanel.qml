import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
Rectangle {
    id: editingPanel
    SplitView.preferredWidth: parent.width * 0.175
    SplitView.minimumWidth: parent.width * 0.11
    color: Constants.background
    RowLayout {
        anchors.right: parent.right
        anchors.top: parent.top
        width: parent.width
        height: 40
        spacing: 3
        LFButton {
            id: mainEditPanel
            text: "Main"
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumWidth: 0
        } //mainEditPanel
        LFButton {
            id: cropRotatePanel
            text: "Crop & Rotate"
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumWidth: 0
        } //cropRotatePanel
        LFButton {
            id: selectionPanel
            text: "Selection Edit"
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumWidth: 0
        } //selectionPanel
    }
}