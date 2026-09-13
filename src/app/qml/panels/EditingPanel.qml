import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: editingPanel

    SplitView.preferredWidth: parent.width * 0.175
    SplitView.minimumWidth: parent.width * 0.11

    color: Constants.background

    property int currentTab: 0

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            Layout.minimumHeight: 40
            Layout.maximumHeight: 40
            spacing: 3

            LFButton {
                id: mainEditPanel
                text: "Main"

                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumWidth: 0

                onClicked: editingPanel.currentTab = 0
            }

            LFButton {
                id: cropRotatePanel
                text: "Crop & Rotate"

                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumWidth: 0

                onClicked: editingPanel.currentTab = 1
            }

            LFButton {
                id: selectionPanel
                text: "Selection Edit"

                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumWidth: 0

                onClicked: editingPanel.currentTab = 2
            }
        }

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            currentIndex: editingPanel.currentTab

            Item {
                id: mainPanel


                Column {
                    anchors.fill: parent
                    Label {
                        height: 20
                        width: parent.width
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.topMargin: 20
                        text: "editing panel"
                        color: Constants.text

                    }
                    LFHorizontalSpacer{}
                    Item {
                        height: 10
                        width: parent.width
                    }
                    Label  {
                        height: 20
                        text: "Light Settings"
                        color: Constants.text
                    }
                    LFHorizontalSpacer{}

                    Slider {
                        from: -5
                        to: 5
                        value: appController.exposure

                        onMoved: {
                            appController.exposure = value
                        }
                    }
                }
            }

            Item {
                id: cropPanel

                Label {
                    anchors.centerIn: parent
                    text: "Crop & Rotate"
                }
            }

            Item {
                id: selectionEditPanel

                Label {
                    anchors.centerIn: parent
                    text: "Selection Edit"
                }
            }
        }
    }
}