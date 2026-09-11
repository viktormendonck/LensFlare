import QtQuick
import QtQuick.Controls
MenuBar {
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
        } //menubar styling
        delegate: MenuBarItem {
            contentItem: Text {
                text: parent.text
                color: "white"
            }
            background: Rectangle {
                color: parent.highlighted
                       ? Constants.background
                       : Constants.accentBackground
                height: parent.height -2
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
        } //menubar item styling
        Menu {
            title: "File"
            MenuItem {
                text: "Open"

                onTriggered: appController.OpenFileButton()
                LSToolTip{
                    text: "Ctrl+O"
                    visible: parent.hovered
                }
            }
            MenuItem {
                text: "Save"
                onTriggered: appController.SaveButton()
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
                onTriggered: appController.UndoButton()
                LSToolTip{
                    text: "Ctrl+Z"
                    visible: parent.hovered
                }
            }
            MenuItem {
                text: "Redo"
                onTriggered: appController.RedoButton()
                LSToolTip{
                    text: "Ctrl+Y"
                    visible: parent.hovered
                }
            }
        }
    } //id: menuBar