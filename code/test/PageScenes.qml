/**
 * @file PageScenes.qml
 * @brief Interface de déclenchement des scènes DMX préenregistrées.
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: pageScenes

    /** @name Paramètres de Design UI */
    property int baseFontSize: 18
    property int titleFontSize: 32
    property int topBarBtnHeight: 50
    property color commonBtnColor: "#2a3449"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        anchors.topMargin: 50
        spacing: 20

        RowLayout {
            Layout.fillWidth: true

            Text {
                text: "⚙️ Scènes"
                color: "white"
                font.pixelSize: titleFontSize
                font.bold: true
                Layout.fillWidth: true
            }

            Rectangle {
                color: commonBtnColor
                radius: 5
                width: 150
                height: topBarBtnHeight

                Text {
                    text: "Rafraîchir"
                    color: "white"
                    font.bold: true
                    font.pixelSize: baseFontSize
                    anchors.centerIn: parent
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: myNetwork.fetchScenes()
                }
            }

            Rectangle {
                Layout.leftMargin: 10
                color: commonBtnColor
                radius: 5
                width: 100
                height: topBarBtnHeight
                Text {
                    text: "Retour"
                    color: "white"
                    font.bold: true
                    font.pixelSize: baseFontSize
                    anchors.centerIn: parent
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: pageScenes.StackView.view.pop()
                }
            }
        }

        GridView {
            id: sceneGrid
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            cellWidth: sceneGrid.width / 2
            cellHeight: 160
            model: dbSceneModel

            delegate: Rectangle {
                width: sceneGrid.cellWidth - 15
                height: 145
                color: "#273147"
                radius: 10
                border.color: "#3a4a66"
                border.width: 1

                property int sceneId: model.idScene !== undefined ? model.idScene : -1

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 15

                    Text {
                        text: model.nomScene !== undefined ? model.nomScene : "Chargement..."
                        color: "white"
                        font.bold: true
                        font.pixelSize: baseFontSize
                        Layout.fillWidth: true
                        wrapMode: Text.Wrap
                    }

                    Item { Layout.fillHeight: true }

                    Rectangle {
                        color: confirmTimer.running ? "#00a86b" : "#5d5cff"
                        radius: 8
                        Layout.fillWidth: true
                        Layout.preferredHeight: 60

                        Text {
                            text: confirmTimer.running ? "✔ Lancée" : "Lancer"
                            color: "white"
                            font.bold: true
                            font.pixelSize: baseFontSize
                            anchors.centerIn: parent
                        }

                        Timer { id: confirmTimer; interval: 500 }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                if (sceneId !== -1) {
                                    myNetwork.lancerSceneTCP(sceneId)
                                    myNetwork.fetchSceneValues(sceneId)
                                    confirmTimer.start()
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
