import QtQuick
import QtQuick.Controls

ToolTip {
    id: root
    background: Rectangle {
        color: Constants.accentBackground
        opacity: 0.8
        radius: 4
        border.width:1
        border.color:Constants.spacer
    }

    contentItem: Text {
        text: root.text
        opacity: 0.8
        color: Constants.toolTipText
    }
}
