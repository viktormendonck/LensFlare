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
                color: Constants.text
            }
            background: Rectangle {
                color: parent.pressed
                       ? Constants.selectedColor
                       : parent.hovered
                         ? Constants.background
                         : Constants.accentBackground
                border.color: parent.hovered
                                 ? Constants.selectedOutlineColor
                                 : Constants.outlineColor
                border.width: 1
                radius: 2
                height: parent.height -2

            }
        } //menubar item styling

        spacing: 4
        LFMenu {
            title: "File"
            LFMenuItem {
                text: "Open"

                onTriggered: appController.OpenFileButton()
                LFToolTip{
                    text: "Ctrl+O"
                    visible: parent.hovered
                }
            }
            LFMenuItem {
                text: "Save"
                onTriggered: appController.SaveButton()
                LFToolTip{
                    text: "Ctrl+S"
                    visible: parent.hovered
                }
            }
        }
        LFMenu {
            title: "Edit"
            LFMenuItem {
                text: "Undo"
                onTriggered: appController.UndoButton()
                LFToolTip{
                    text: "Ctrl+Z"
                    visible: parent.hovered
                }
            }
            LFMenuItem {
                text: "Redo"
                onTriggered: appController.RedoButton()
                LFToolTip{
                    text: "Ctrl+Y"
                    visible: parent.hovered
                }
            }
        }
    } //id: menuBar