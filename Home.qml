import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.qmlmodels 1.0
import app.Controller 1.0

Pane {
    id: mainItm
    Controller {
        id: controller
    }

    ColumnLayout {
        anchors.fill: parent
        Label{
            text: "UDP Test"
            Layout.alignment: Qt.AlignHCenter
            font.pixelSize: 30
            font.bold: true
        }
        Rectangle{
            color: "White"
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: 660
            Layout.preferredHeight:  4
        }

        RowLayout{
            Item{Layout.fillWidth: true}
            Button{
                text: "Start"
                font.pixelSize: 21
                onClicked: {
                    controller.startUDP()
                    startLbl.text = "Started!"
                }
            }
            Label{
                id: startLbl
                font.pixelSize: 19
            }
            Item{Layout.fillWidth: true}
        }
        RowLayout{
            Item{Layout.fillWidth: true}
            Label{
                text: "Data Read: "
                font.pixelSize: 21
            }
            Label{
                text: controller.udpData
                font.pixelSize: 21
            }
            Item{Layout.fillWidth: true}
        }
        RowLayout{
            Item{Layout.fillWidth: true}
            TextField{
                id: sendTxt
                font.pixelSize: 21
            }
            Button{
                text: "Send"
                font.pixelSize: 21
                onClicked: {
                    controller.writeDataEthernet(sendTxt.text)
                }
            }
            Item{Layout.fillWidth: true}
        }
    }
}
