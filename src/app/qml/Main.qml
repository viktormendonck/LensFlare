import QtQuick
import QtQuick.Controls

ApplicationWindow {
    width: 1920
    height: 1080

    visible: true
    title: "Lensflare dev"
    MainMenuBar {
        id: menuBar
    } //id: menuBar
    SplitView {
        id: verticalSplitView

        anchors.top: menuBar.bottom
        anchors.bottom: statusBar.top
        width:parent.width

        orientation: Qt.Vertical
        handle: Rectangle {
            implicitHeight: 5
            color: Constants.background
            Rectangle {
                anchors.centerIn: parent
                width: parent.width
                height: 2
                color: Constants.spacer
            }
        } // make the handle wider

        SplitView {
            SplitView.fillHeight: true
            orientation: Qt.Horizontal
            handle: Rectangle {
                implicitWidth: 5
                color: Constants.background
                Rectangle {
                    anchors.centerIn: parent
                    width: 2
                    height: parent.height
                    color: Constants.spacer
                }
            } //make handle wider

            FolderViewPanel {
                id: foldersPanel
            } // id: folderViewPanel

            MainViewPanel {
                id: mainViewPanel
            } // id: mainViewPanel

            EditingPanel {
                id: editingPanel
            } // id: editingPanel
        }
        FilmStripPanel{
            id: filmstripPanel
        } // id: filmstripPanel
    } // id: verticalSplitView

    Rectangle{
        id: statusBar
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        width:parent.width
        height: 26
        color: Constants.accentBackground
        Rectangle {
            height: 2
            width: parent.width
            anchors.bottom: parent.bottom
            color: Constants.spacer
        }// spacer bar
        Label {
            anchors.centerIn: parent
            opacity: Constants.commentTextOpacity
            text: appController.statusText
        }
    } //id statusBar
}
