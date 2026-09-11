import QtQuick
import QtQuick.Templates as T
//explainer for self, starting from a template
// allows all the base logic of an object to be stripped
// in this case it removes any and all highlighting on hover or anything else that was causing issues before

T.MenuItem {
    id: control

    leftPadding: 10
    rightPadding: 10
    topPadding: 6
    bottomPadding: 6

    implicitWidth: contentItem.implicitWidth
                   + leftPadding
                   + rightPadding

    implicitHeight: contentItem.implicitHeight
                    + topPadding
                    + bottomPadding

    contentItem: Text {
        text: control.text
        color: Constants.text

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft

        elide: Text.ElideRight
    }

    background: Rectangle {
        color: control.pressed
               ? Constants.selectedColor
               : control.highlighted || control.hovered
                 ? Constants.background
                 : Constants.accentBackground

        border.color: control.highlighted || control.hovered
                      ? Constants.selectedOutlineColor
                      : Constants.outlineColor

        border.width: 1
        radius: 2
    }
}