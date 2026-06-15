import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import com.dmx.network 1.0

ApplicationWindow {
    id: mainWindow

    visibility: Window.FullScreen
    contentOrientation: Qt.LandscapeOrientation

    width: 800
    height: 480
    title: "Tablette DMX"
    color: "#1c2331"

    property int menuBtnWidth: 300
    property int menuBtnHeight: 65
    property int menuIconSize: 24
    property int menuTextSize: 20

    property string serverIP: "192.168.1.20"
    property int tcpPort: 12345
    property bool isConnected: myNetwork.isConnected

    property var allUniverses: []
    property var universeNames: []
    property var savedSliderValues: ({})

    ListModel { id: dbSceneModel }
    ListModel { id: logModel } // Modèle pour le moniteur de diagnostic


    NetworkManager {
        id: myNetwork

        onConnectionChanged: {
            if (myNetwork.isConnected) {
                myNetwork.fetchUniverses()
                myNetwork.fetchScenes()
            }
        }

        // Réception des logs détaillés du C++ pour le volet de diagnostic
        onMessageLogged: function(direction, content) {
            logModel.insert(0, {
                                "dir": direction,
                                "msg": content,
                                "time": new Date().toLocaleTimeString()
                            })
            if (logModel.count > 50) logModel.remove(50) // Garder l'historique des 50 derniers messages
        }
    }

    Connections {
        target: myNetwork

        function onUniversesReceived(data) {
            allUniverses = data
            var names = []

            for (var i = 0; i < data.length; i++) {
                // On récupère le nom de l'univers
                var nom = data[i].nom_univers !== undefined ? data[i].nom_univers : "Univers"

                // On récupère l'adresse IP (selon ta base de données)
                var ip = data[i].adresseIp !== undefined ? data[i].adresseIp : ""

                // Si on a une IP, on l'affiche entre parenthèses à côté du nom
                if (ip !== "") {
                    names.push(nom + " (" + ip + ")")
                } else {
                    names.push(nom)
                }
            }
            universeNames = names
        }

        function onScenesReceived(scenesList) {
            dbSceneModel.clear()
            for (var i = 0; i < scenesList.length; i++) {
                dbSceneModel.append(scenesList[i])
            }
        }

        function onSceneValuesReceived(valuesList) {
            var currentMemory = mainWindow.savedSliderValues
            var nextMemory = {}
            var affectedUniverses = {}

            for (var i = 0; i < valuesList.length; i++) {
                affectedUniverses[valuesList[i].idUnivers.toString()] = true
            }

            var keys = Object.keys(currentMemory)
            for (var k = 0; k < keys.length; k++) {
                var keyName = keys[k]
                var univPrefix = keyName.split("_")[0]

                if (!affectedUniverses[univPrefix]) {
                    nextMemory[keyName] = currentMemory[keyName]
                }
            }

            for (var j = 0; j < valuesList.length; j++) {
                var item = valuesList[j]
                var newKey = item.idUnivers + "_" + item.idEquipement + "_" + item.numeroCanal
                nextMemory[newKey] = item.valeurCanaux
            }

            mainWindow.savedSliderValues = nextMemory
        }
    }

    /** @brief Watchdog : Reconnexion TCP toutes les 5 secondes si déconnecté. */
    Timer {
        interval: 5000; running: true; repeat: true;
        onTriggered: if (!myNetwork.isConnected) myNetwork.connectToTcpServer(serverIP, tcpPort)
    }

    Component.onCompleted: {
        myNetwork.connectToTcpServer(serverIP, tcpPort)
    }

    // ========================================================
    // 🔍 LE VOLET DE DIAGNOSTIC RÉSEAU (Drawer)
    // ========================================================
    Drawer {
        id: diagnosticDrawer
        width: parent.width / 1.8 // Largeur pour une bonne visibilité du JSON
        height: parent.height
        edge: Qt.RightEdge
        interactive: true

        background: Rectangle { color: "#0d1117"; opacity: 0.98 }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 10

            Text {
                text: "📡 Moniteur Réseau"
                color: "white"; font.bold: true; font.pixelSize: 22
                Layout.alignment: Qt.AlignHCenter
            }

            ListView {
                id: logListView
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                model: logModel
                spacing: 10

                delegate: ColumnLayout {
                    width: logListView.width - 20
                    spacing: 4

                    RowLayout {
                        spacing: 10
                        Text { text: model.time; color: "gray"; font.pixelSize: 10 }
                        Rectangle {
                            width: 45; height: 18; radius: 3
                            color: model.dir === "SEND" ? "#5d5cff" : (model.dir === "RECV" ? "#00a86b" : "#444")
                            Text {
                                text: model.dir
                                color: "white"
                                anchors.centerIn: parent
                                font.pixelSize: 9
                                font.bold: true
                            }
                        }
                    }

                    // Affichage complet du message (JSON ou commande) avec retour à la ligne
                    Text {
                        text: model.msg
                        color: model.dir === "RECV" ? "#00ff00" : "white"
                        font.family: "monospace"
                        font.pixelSize: 11
                        Layout.fillWidth: true
                        wrapMode: Text.Wrap
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 1
                        color: "#222"
                    }
                }
            }

            Button {
                text: "Fermer"
                Layout.fillWidth: true
                onClicked: diagnosticDrawer.close()
            }
        }
    }

    // ========================================================
    // 🎛️ NAVIGATION ET BOUTON D'ACCÈS AU DIAGNOSTIC
    // ========================================================

    // Bouton flottant pour ouvrir le diagnostic, visible PARTOUT (ligne modifiée ici)
    Rectangle {
        width: 50; height: 50; radius: 25
        color: "#5d5cff"; opacity: 0.8
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 20
        z: 100

        Text { text: "🔍"; anchors.centerIn: parent; font.pixelSize: 24 }
        MouseArea { anchors.fill: parent; onClicked: diagnosticDrawer.open() }
    }

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: menuPage
    }

    // ========================================================
    // 🏠 COMPOSANT : PAGE DU MENU PRINCIPAL
    // ========================================================
    Component {
        id: menuPage
        Item {
            ColumnLayout {
                anchors.centerIn: parent
                spacing: 25

                Text { text: "🎛️"; font.pixelSize: 100; Layout.alignment: Qt.AlignHCenter }
                Text { text: "Tablette DMX"; color: "white"; font.pixelSize: 36; font.bold: true; Layout.alignment: Qt.AlignHCenter }

                Rectangle {
                    id: connectionIndicator
                    Layout.preferredWidth: 200
                    Layout.preferredHeight: 40
                    radius: 20
                    color: mainWindow.isConnected ? "#1E3A1E" : "#3A1E1E"
                    border.color: mainWindow.isConnected ? "#00a86b" : "#ff3333"
                    border.width: 2
                    Layout.alignment: Qt.AlignHCenter

                    Row {
                        anchors.centerIn: parent
                        spacing: 10
                        Rectangle {
                            width: 14; height: 14; radius: 7
                            color: parent.parent.border.color
                            anchors.verticalCenter: parent.verticalCenter
                        }
                        Text {
                            text: mainWindow.isConnected ? "CONNECTÉ" : "DÉCONNECTÉ"
                            color: parent.parent.border.color
                            font.bold: true; font.pixelSize: 14
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }
                }

                Rectangle {
                    Layout.preferredWidth: menuBtnWidth
                    Layout.preferredHeight: menuBtnHeight
                    radius: 12
                    color: "#5d5cff"
                    Layout.alignment: Qt.AlignHCenter

                    RowLayout {
                        anchors.centerIn: parent
                        spacing: 15
                        Text { text: "⚙️"; color: "white"; font.pixelSize: menuIconSize }
                        Text { text: "Scènes"; color: "white"; font.bold: true; font.pixelSize: menuTextSize }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: stackView.push("PageScenes.qml")
                    }
                }

                Rectangle {
                    Layout.preferredWidth: menuBtnWidth
                    Layout.preferredHeight: menuBtnHeight
                    radius: 12
                    color: "#00a86b"
                    Layout.alignment: Qt.AlignHCenter

                    RowLayout {
                        anchors.centerIn: parent
                        spacing: 15
                        Text { text: "💡"; color: "white"; font.pixelSize: menuIconSize }
                        Text { text: "Contrôle"; color: "white"; font.bold: true; font.pixelSize: menuTextSize }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: stackView.push("PageControle.qml")
                    }
                }

                Rectangle {
                    Layout.preferredWidth: menuBtnWidth
                    Layout.preferredHeight: menuBtnHeight
                    radius: 12
                    color: "#d9534f"
                    Layout.alignment: Qt.AlignHCenter

                    Text {
                        text: "Quitter"
                        color: "white"
                        font.bold: true
                        font.pixelSize: menuTextSize
                        anchors.centerIn: parent
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: Qt.quit()
                    }
                }
            }
        }
    }
}
