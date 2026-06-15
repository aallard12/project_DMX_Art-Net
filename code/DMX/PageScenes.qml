import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

/**
 * @file PageScenes.qml
 * @brief Interface de gestion et de lancement des scènes DMX.
 * @author Valentin
 * @date 2026
 * @details Cette page affiche sous forme de grille les scènes DMX enregistrées
 * dans la base de données. Elle permet à l'utilisateur de filtrer ces scènes
 * par univers et de déclencher leur exécution sur le réseau via le NetworkManager.
 */

Item {
    id: pageScenes

    /**
     * @property int baseFontSize
     * @brief Taille de police standard pour le texte des cartes de scènes.
     */
    property int baseFontSize: 18

    /**
     * @property int titleFontSize
     * @brief Taille de police pour l'en-tête de la page.
     */
    property int titleFontSize: 32

    /**
     * @property int topBarBtnHeight
     * @brief Hauteur unifiée pour les éléments de la barre de navigation (ComboBox, Boutons).
     */
    property int topBarBtnHeight: 50

    /**
     * @property color commonBtnColor
     * @brief Couleur de fond par défaut des éléments interactifs inactifs.
     */
    property color commonBtnColor: "#2a3449"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        anchors.topMargin: 50
        spacing: 20

        // --- BARRE SUPÉRIEURE ---
        RowLayout {
            Layout.fillWidth: true

            Text {
                text: "⚙️ Scènes"
                color: "white"
                font.pixelSize: titleFontSize
                font.bold: true
            }

            Item { Layout.fillWidth: true }

            /**
             * @brief Sélecteur de filtrage des scènes par Univers.
             * @details Permet d'isoler les scènes qui n'affectent qu'un univers spécifique.
             * Lors d'un changement, une nouvelle requête JSON `get_scenes` est envoyée au serveur.
             */
            ComboBox {
                id: universeComboBox
                Layout.leftMargin: 10
                Layout.preferredWidth: 250
                Layout.preferredHeight: topBarBtnHeight

                // Concaténation pour avoir "Tous les univers" en premier
                model: ["Tous les univers"].concat(mainWindow.universeNames)

                background: Rectangle {
                    color: commonBtnColor
                    radius: 5
                }
                contentItem: Text {
                    leftPadding: 15
                    text: universeComboBox.currentText
                    color: "white"
                    font.pixelSize: baseFontSize
                    font.bold: true
                    verticalAlignment: Text.AlignVCenter
                }

                // Action quand on change la sélection
                onActivated: function(index) {
                    var realUnivId = -1; // Par défaut : Tous les univers

                    // 💡 [EXPLICATION] : Puisque l'index 0 est "Tous les univers", on regarde
                    // l'index - 1 dans le vrai tableau de la base de données.
                    if (index > 0) {
                        var uniData = mainWindow.allUniverses[index - 1];

                        // Sécurité : On récupère l'ID exact de la base de données
                        realUnivId = uniData.idUnivers !== undefined ? uniData.idUnivers : 1;
                    }

                    // On sauvegarde le choix pour le Timer global
                    mainWindow.currentSceneFilterUniverseId = realUnivId;

                    // 💡 [EXPLICATION] : On déclenche la demande de filtrage côté réseau !
                    myNetwork.fetchScenes(realUnivId);
                }
            }

            Rectangle {
                Layout.leftMargin: 10
                color: commonBtnColor
                radius: 5
                width: 100
                height: topBarBtnHeight
                Text { text: "Retour"; color: "white"; font.bold: true; font.pixelSize: baseFontSize; anchors.centerIn: parent }
                MouseArea { anchors.fill: parent; onClicked: pageScenes.StackView.view.pop() }
            }
        }

        // --- GRILLE DES SCÈNES ---

        /**
         * @brief Grille d'affichage dynamique des scènes.
         * @details Se met à jour automatiquement lorsque le modèle `dbSceneModel`
         * reçoit de nouvelles données via le signal `scenesReceived` du C++.
         */
        GridView {
            id: sceneGrid
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            // 💡 [EXPLICATION] : Largeur dynamique = exactement la moitié de l'écran par carte (2 colonnes)
            cellWidth: sceneGrid.width / 2
            cellHeight: 160
            model: dbSceneModel

            /**
             * @brief Composant visuel (Carte) pour une scène individuelle.
             */
            delegate: Rectangle {
                width: sceneGrid.cellWidth - 15
                height: 145
                color: "#273147"
                radius: 10
                border.color: "#3a4a66"
                border.width: 1

                /**
                 * @property int sceneId
                 * @brief Identifiant unique de la scène en base de données.
                 */
                // 💡 [EXPLICATION] : Dans un composant delegate, "model" accède aux propriétés JSON du serveur.
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

                    Item { Layout.fillHeight: true } // 💡 [EXPLICATION] : Ressort invisible pour pousser le bouton en bas de la carte.

                    /**
                     * @brief Bouton de lancement de la scène.
                     * @details Envoie deux commandes réseau séquentielles :
                     * 1. Demande d'exécution matérielle (`lancerSceneTCP`).
                     * 2. Demande de récupération des valeurs (`fetchSceneValues`) pour
                     * mettre à jour la mémoire locale des curseurs tactiles.
                     */
                    Rectangle {
                        // 💡 [EXPLICATION] : Si le timer est en cours, le bouton est vert, sinon il est bleu.
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

                        /**
                         * @brief Timer gérant le retour visuel (couleur verte) du bouton après un clic.
                         */
                        Timer { id: confirmTimer; interval: 500 }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                if (sceneId !== -1) {
                                    // 💡 [EXPLICATION] : 1. Ordre de jouer la lumière
                                    myNetwork.lancerSceneTCP(sceneId)
                                    // 💡 [EXPLICATION] : 2. Demande des valeurs pour mettre nos curseurs à jour
                                    myNetwork.fetchSceneValues(sceneId)
                                    // 💡 [EXPLICATION] : 3. Lancement de l'animation verte (0.5s)
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
