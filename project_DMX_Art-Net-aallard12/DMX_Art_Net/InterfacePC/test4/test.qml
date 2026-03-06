import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    visible: true
    width: 1000; height: 700
    title: "DMX Equipment Manager"

    readonly property color bgColor: "#0a0e1a"
    readonly property color surfaceColor: "#131829"
    readonly property color primaryColor: "#6366f1"

    background: Rectangle { color: bgColor }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        // SIDEBAR
        Rectangle {
            Layout.fillHeight: true
            Layout.preferredWidth: 240
            color: surfaceColor

            ColumnLayout {
                anchors.fill: parent; anchors.margins: 20
                Text { text: "DMX MANAGER"; color: "white"; font.bold: true; font.pixelSize: 20 }

                Button {
                    text: "Équipements"; Layout.fillWidth: true
                    onClicked: mainStack.currentIndex = 0
                }
                Item { Layout.fillHeight: true }
            }
        }

        // MAIN CONTENT
        StackLayout {
            id: mainStack
            Layout.fillWidth: true

            // PAGE 1: LISTE
            ColumnLayout {
                anchors.margins: 30
                RowLayout {
                    Text { text: "Équipements"; color: "white"; font.pixelSize: 28; font.bold: true }
                    Item { Layout.fillWidth: true }
                    Button { text: "+ Ajouter"; onClicked: mainStack.currentIndex = 1 }
                }

                ListView {
                    Layout.fillWidth: true; Layout.fillHeight: true
                    model: dmxStore.equipments
                    clip: true
                    spacing: 10
                    delegate: Rectangle {
                        width: parent.width; height: 60
                        color: "#1e293b"; radius: 8
                        RowLayout {
                            anchors.fill: parent; anchors.margins: 15
                            Text { text: modelData.name; color: "white"; font.bold: true }
                            Item { Layout.fillWidth: true }
                            Button {
                                text: "Supprimer";
                                onClicked: dmxStore.deleteEquipment(index)
                            }
                        }
                    }
                }
            }

            // PAGE 2: FORMULAIRE
            ColumnLayout {
                anchors.margins: 30
                TextField { id: nameInput; placeholderText: "Nom de l'équipement"; Layout.fillWidth: true }
                Button {
                    text: "Enregistrer"
                    onClicked: {
                        dmxStore.saveEquipment(nameInput.text, [])
                        nameInput.text = ""
                        mainStack.currentIndex = 0
                    }
                }
                Button { text: "Retour"; onClicked: mainStack.currentIndex = 0 }
                Item { Layout.fillHeight: true }
            }
        }
    }
}
