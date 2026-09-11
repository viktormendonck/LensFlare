import QtQuick
import QtQuick.Controls

ApplicationWindow {
    width: 1920
    height: 1080
    visible: true
    title: "Lensflare dev"
    MenuBar {
        id: menuBar
        height: 26
        width: parent.width

        background: Rectangle {
            color: Constants.accentBackground
            Rectangle {
                height: 2
                width: parent.width
                anchors.bottom: parent.bottom
                color: Constants.spacer
            }// spacer bar
        } //menubar background
        delegate: MenuBarItem {
            contentItem: Text {
                text: parent.text
                color: "white"
            }
            background: Rectangle {
                color: parent.highlighted
                       ? Constants.background
                       : Constants.accentBackground
                Rectangle{
                    width:1
                    height: parent.height
                    color: Constants.spacer
                    anchors.top : parent.top
                    anchors.left: parent.left
                }
                Rectangle{
                    width:1
                    height: parent.height
                    color: Constants.spacer
                    anchors.top : parent.top
                    anchors.right: parent.right
                }
            }
        } //menubar styling

        Menu {
            title: "File"
            MenuItem {
                text: "Open"
                LSToolTip{
                    text: "Ctrl+O"
                    visible: parent.hovered
                }
            }
            MenuItem {
                text: "Save"
                LSToolTip{
                    text: "Ctrl+S"
                    visible: parent.hovered
                }
            }
        }
        Menu {
            title: "Edit"
            MenuItem {
                text: "Undo"
                LSToolTip{
                    text: "Ctrl+Z"
                    visible: parent.hovered
                }
            }
            MenuItem {
                text: "Redo"
                LSToolTip{
                    text: "Ctrl+Y"
                    visible: parent.hovered
                }
            }
        }
    }
    SplitView {
        anchors.top: menuBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
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
        }

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
            }

            Rectangle {
                id: foldersPanel
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
        }
        Rectangle{
            id: filmstripPanel
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
    }
}
