import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: pageDiagnostic

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15

        // --- EN-TÊTE ---
        RowLayout {
            Layout.fillWidth: true

            Text {
                text: "🛠️ Console de Diagnostic"
                color: "white"
                font.pixelSize: 32
                font.bold: true
                Layout.fillWidth: true
            }

            Rectangle {
                color: "#2a3449"
                radius: 5
                width: 120
                height: 50
                Text { text: "Effacer"; color: "white"; font.bold: true; anchors.centerIn: parent }
                MouseArea { anchors.fill: parent; onClicked: consoleLog.text = "=== DÉBUT DU LOG ===\n" }
            }

            Rectangle {
                Layout.leftMargin: 10
                color: "#d9534f"
                radius: 5
                width: 100
                height: 50
                Text { text: "Retour"; color: "white"; font.bold: true; anchors.centerIn: parent }
                MouseArea { anchors.fill: parent; onClicked: pageDiagnostic.StackView.view.pop() }
            }
        }

        // --- BOUTONS DE TEST ---
        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            Button {
                text: "📡 Tester Univers"
                onClicked: {
                    consoleLog.text += "\n[ENVOI] Demande des univers (fetchUniverses)..."
                    myNetwork.fetchUniverses()
                }
            }
            Button {
                text: "🎬 Tester Scènes"
                onClicked: {
                    consoleLog.text += "\n[ENVOI] Demande des scènes (fetchScenes)..."
                    myNetwork.fetchScenes()
                }
            }
        }

        // --- ÉCRAN DE LA CONSOLE ---
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#0d1117"
            border.color: "#3a4a66"
            border.width: 2
            radius: 5

            ScrollView {
                anchors.fill: parent
                anchors.margins: 10
                clip: true

                TextArea {
                    id: consoleLog
                    text: "=== DÉBUT DU LOG ===\n"
                    color: "#00ff00" // Vert style "Matrix/Terminal"
                    font.family: "monospace"
                    font.pixelSize: 14
                    readOnly: true
                    wrapMode: Text.WrapAnywhere
                }
            }
        }
    }

    // --- ÉCOUTEURS POUR CAPTER LES RÉPONSES DU SERVEUR ---
    Connections {
        target: myNetwork

        function onUniversesReceived(data) {
            consoleLog.text += "\n[REÇU] Universes : " + data.length + " trouvés.\n"
            // Affiche le JSON brut (limité à 500 caractères pour ne pas faire lagger la tablette)
            consoleLog.text += JSON.stringify(data, null, 2).substring(0, 500) + "...\n"
        }

        function onScenesReceived(scenesList) {
            consoleLog.text += "\n[REÇU] Scènes : " + scenesList.length + " trouvées.\n"
            consoleLog.text += JSON.stringify(scenesList, null, 2) + "\n"
        }

        function onConnectionChanged() {
            consoleLog.text += "\n[RÉSEAU] État de la connexion : " + (myNetwork.isConnected ? "CONNECTÉ" : "DÉCONNECTÉ")
        }
    }
}
