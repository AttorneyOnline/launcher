import QtQuick.Window 2.15
import QtQuick.Controls.Windows 2.15

ApplicationWindow {
    visible:true

    menuBar: MenuBar {
        Menu {
            title: qsTr("&File")
            Action { text: qsTr("&Open Settings")}
        }

        Menu {
            title: qsTr("&Help")
            Action { text: qsTr("&Version")}
        }
    }

    Button {
        id: happy_button
        text:"Press Me!"
    }
}
