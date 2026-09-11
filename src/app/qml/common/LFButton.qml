import QtQuick
import QtQuick.Controls
Button{
    id: control
    background: Rectangle {
        color: control.pressed
               ? Constants.selectedColor
               : control.hovered
                 ? Constants.background
                 : Constants.accentBackground
        border.color: control.hovered
                         ? Constants.selectedOutlineColor
                         : Constants.outlineColor
        radius: 4
        border.width: 2
    }
    contentItem: Text {
        text: control.text
        font.pixelSize: 12
        color: Constants.text
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}