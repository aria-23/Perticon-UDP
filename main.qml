import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls.Material 2.15

Window {
    id: mainWindow
    width: 700
    minimumWidth: 700
    height: 300


    visible: true
    title: qsTr("Perticon")
    Material.theme: Material.Dark
    Material.accent: "dodgerBlue"

    Home {
        id: homePage
        anchors.fill: parent
    }
}
