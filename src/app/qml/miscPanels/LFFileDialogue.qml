import QtQuick
import QtQuick.Dialogs

Item {
    function open() {
        openFileDialog.open()
    }
    FileDialog {
        id: openFileDialog

        title: "Open RAW image"


        nameFilters: [
            "RAW images (*.CR2 *.CR3 *.NEF *.NRW *.ARW *.RAF *.DNG *.ORF *.RW2 *.PEF)",
            "All files (*)"
        ]

        onAccepted: {
            appController.OpenFile(selectedFile)
        }
    }
}