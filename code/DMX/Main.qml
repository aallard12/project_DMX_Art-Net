import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import com.dmx.network 1.0
import "dmxLogic.js" as DmxLogic

/**
 * @file Main.qml
 * @brief Fenêtre principale et point d'entrée de l'interface utilisateur.
 * @author Valentin
 * @date 2026
 * @details Ce composant racine gère la navigation (StackView), l'état global
 * de l'application (dictionnaire des valeurs, liste des équipements),
 * et la communication asynchrone continue avec le backend C++ (NetworkManager).
 */
// 💡 [EXPLICATION] : ApplicationWindow est la fenêtre physique de ton application.
// C'est le composant "parent" de tout le reste.
ApplicationWindow {
    id: mainWindow

    // 💡 [EXPLICATION] : Force l'affichage en plein écran et en mode paysage sur la tablette.
    visibility: Window.FullScreen
    contentOrientation: Qt.LandscapeOrientation

    width: 800
    height: 480
    title: "Tablette DMX"
    color: "#1c2331"

    // --- PROPRIÉTÉS GLOBALES (Accessibles depuis toutes les pages) ---
    // 💡 [EXPLICATION] : Les "property" définies ici sont globales. N'importe quel autre
    // fichier QML pourra y accéder en tapant "mainWindow.nomDeLaPropriete".

    /** @property int menuBtnWidth Largeur standardisée des boutons du menu principal. */
    property int menuBtnWidth: 300
    /** @property int menuBtnHeight Hauteur standardisée des boutons du menu principal. */
    property int menuBtnHeight: 65
    /** @property int menuIconSize Taille de police pour les icônes (émojis) du menu. */
    property int menuIconSize: 24
    /** @property int menuTextSize Taille de police pour les textes du menu. */
    property int menuTextSize: 20

    /** @property string serverIP Adresse IP du serveur Raspberry Pi (codée en dur pour le prototype). */
    property string serverIP: "192.168.1.20"
    /** @property int tcpPort Port d'écoute du serveur TCP. */
    property int tcpPort: 12345

    /** @property bool isConnected Raccourci vers l'état de connexion réseau du backend C++. */
    property bool isConnected: myNetwork.isConnected

    /** @property int currentSceneFilterUniverseId Mémorise l'univers sélectionné pour filtrer les scènes. */
    property int currentSceneFilterUniverseId: -1
    /** @property int currentControlFilterUniverseId Mémorise l'univers sélectionné pour filtrer les équipements (Contrôle). */
    property int currentControlFilterUniverseId: -1

    /** @property var allUniverses Arborescence complète (Univers > Équipements > Canaux) reçue du serveur. */
    property var allUniverses: []
    /** @property var universeNames Tableau listant uniquement les noms des univers pour les ComboBox. */
    property var universeNames: []
    /** * @property var savedSliderValues
     * @brief Dictionnaire de la mémoire locale DMX.
     * @details Stocke la valeur de chaque canal sous la forme "idUniv_idProj_idCanal": valeur.
     * Garantit la fluidité de l'interface avant même l'envoi réseau.
     */
    property var savedSliderValues: ({})

    /** @brief Modèle de données global contenant la liste des scènes disponibles. */
    // 💡 [EXPLICATION] : Un ListModel est une liste optimisée par le moteur C++ de Qt.
    // Il permet d'afficher des centaines d'éléments sans faire ramer la tablette.
    ListModel { id: dbSceneModel }

    /**
     * @brief Fonction de nettoyage global de la mémoire.
     * @details Appelée lors d'une déconnexion réseau ou d'une déconnexion utilisateur
     * pour garantir qu'aucune ancienne donnée "fantôme" ne reste affichée.
     */
    function clearAllData() {
        mainWindow.allUniverses = []
        mainWindow.universeNames = []
        mainWindow.savedSliderValues = ({})
        mainWindow.currentSceneFilterUniverseId = -1
        dbSceneModel.clear()
    }

    /**
     * @brief Instance unique du backend réseau C++.
     * @details Gère les événements natifs de changement d'état de la socket TCP.
     */
    // 💡 [EXPLICATION] : C'est ici qu'on "invoque" la classe C++ créée dans main.cpp.
    // Toute l'interface réseau passera désormais par cet objet "myNetwork".
    NetworkManager {
        id: myNetwork
        onConnectionChanged: {
            if (myNetwork.isConnected) {
                // Si le réseau revient, on redemande automatiquement les infos au Pi.
                myNetwork.fetchUniverses()
                myNetwork.fetchScenes()
            } else {
                // SI CONNEXION PERDUE : On vide tout pour ne pas voir d'anciennes valeurs
                clearAllData()
            }
        }
    }

    /**
     * @brief Écoute des signaux de réception de données émis par le NetworkManager.
     * @details Parse les listes QVariantList en objets JS et met à jour la mémoire globale.
     */
    // 💡 [EXPLICATION] : Connections permet de brancher l'interface visuelle sur les "signals" du C++.
    Connections {
        target: myNetwork

        /**
         * @brief Traitement de l'arborescence des univers et équipements.
         */
        // 💡 [EXPLICATION] : onUniversesReceived s'active quand le C++ fait `emit universesReceived`.
        function onUniversesReceived(data) {
            if (!isConnected) return; // Sécurité

            mainWindow.savedSliderValues = DmxLogic.extractLiveValues(mainWindow.savedSliderValues, data)

            if (mainWindow.allUniverses.length === 0) {
                mainWindow.allUniverses = data
                mainWindow.universeNames = DmxLogic.formatUniverseNames(data)
            }
        }

        /**
         * @brief Traitement de la liste des scènes.
         */
        function onScenesReceived(scenesList) {
            if (!isConnected) return;

            // On vide systématiquement la liste et on la reremplit avec les nouvelles données.
            dbSceneModel.clear()
            for (var i = 0; i < scenesList.length; i++) {
                dbSceneModel.append(scenesList[i])
            }
        }

        /**
         * @brief Mise à jour de la mémoire locale lors de la réception des valeurs d'une scène.
         */
        function onSceneValuesReceived(valuesList) {
            mainWindow.savedSliderValues = DmxLogic.processSceneValues(mainWindow.savedSliderValues, valuesList)
        }
    }

    // --- TIMERS ---

    /**
     * @brief Watchdog de reconnexion TCP.
     * @details Tente de rétablir la connexion toutes les 5 secondes si celle-ci est perdue.
     */
    // 💡 [EXPLICATION] : Ce timer (Chien de garde) tourne en boucle invisiblement.
    // Il garantit que l'application tentera toujours de se reconnecter en cas de coupure WiFi.
    Timer {
        interval: 5000
        running: true
        repeat: true
        onTriggered: {
            if (!myNetwork.isConnected) {
                myNetwork.connectToTcpServer(mainWindow.serverIP, mainWindow.tcpPort)
            }
        }
    }

    /**
     * @brief Timer de rafraîchissement ("Polling") optionnel.
     * @details Actualise la liste des univers et des scènes toutes les secondes.
     */
    Timer {
        id: autoRefreshTimer
        interval: 1000
        running: myNetwork.isConnected
        repeat: true
        onTriggered: {
            myNetwork.fetchUniverses()
            myNetwork.fetchScenes(mainWindow.currentSceneFilterUniverseId)
        }
    }

    // --- SYSTÈME DE NAVIGATION ---

    /**
     * @brief Pile de navigation gérant l'affichage des différentes pages.
     */
    // 💡 [EXPLICATION] : Le StackView gère l'historique des pages.
    // Au démarrage (initialItem), il charge la page de connexion.
    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: loginPageComponent
    }

    /**
     * @brief Composant encapsulant la page de connexion.
     */
    Component {
        id: loginPageComponent
        PageLogin {
            // 💡 [EXPLICATION] : On écoute le signal personnalisé "loginSuccess" créé dans PageLogin.qml.
            onLoginSuccess: {
                // "replace" écrase la page de login pour qu'on ne puisse pas y retourner en faisant "retour".
                stackView.replace(menuPage)
            }
        }
    }

    /**
     * @brief Composant encapsulant le menu principal (Dashboard).
     */
    Component {
        id: menuPage
        Item {
            objectName: "menuPage"
            ColumnLayout {
                anchors.centerIn: parent
                spacing: 25

                Text { text: "🎛️"; font.pixelSize: 100; Layout.alignment: Qt.AlignHCenter }
                Text { text: "Tablette DMX"; color: "white"; font.pixelSize: 36; font.bold: true; Layout.alignment: Qt.AlignHCenter }

                // Pastille de connexion
                Rectangle {
                    Layout.preferredWidth: 200
                    Layout.preferredHeight: 40
                    radius: 20
                    Layout.alignment: Qt.AlignHCenter
                    // 💡 [EXPLICATION] : Opérateur ternaire (Condition ? Si Vrai : Si Faux).
                    // Change la couleur en temps réel selon l'état du réseau.
                    color: mainWindow.isConnected ? "#1E3A1E" : "#3A1E1E"
                    border.color: mainWindow.isConnected ? "#00a86b" : "#ff3333"
                    border.width: 2

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

                // Bouton de navigation vers les Scènes
                Rectangle {
                    Layout.preferredWidth: menuBtnWidth; Layout.preferredHeight: menuBtnHeight; radius: 12
                    color: "#5d5cff"; Layout.alignment: Qt.AlignHCenter
                    RowLayout {
                        anchors.centerIn: parent; spacing: 15
                        Text { text: "⚙️"; color: "white"; font.pixelSize: menuIconSize }
                        Text { text: "Scènes"; color: "white"; font.bold: true; font.pixelSize: menuTextSize }
                    }
                    // 💡 [EXPLICATION] : "push" ajoute la page Scènes par-dessus le menu dans l'historique.
                    MouseArea { anchors.fill: parent; onClicked: stackView.push("PageScenes.qml") }
                }

                // Bouton de navigation vers le Contrôle temps réel
                Rectangle {
                    Layout.preferredWidth: menuBtnWidth; Layout.preferredHeight: menuBtnHeight; radius: 12
                    color: "#00a86b"; Layout.alignment: Qt.AlignHCenter
                    RowLayout {
                        anchors.centerIn: parent; spacing: 15
                        Text { text: "💡"; color: "white"; font.pixelSize: menuIconSize }
                        Text { text: "Contrôle"; color: "white"; font.bold: true; font.pixelSize: menuTextSize }
                    }
                    MouseArea { anchors.fill: parent; onClicked: stackView.push("PageControle.qml") }
                }

                // Bouton Déconnexion
                Rectangle {
                    Layout.preferredWidth: menuBtnWidth; Layout.preferredHeight: menuBtnHeight; radius: 12
                    color: "#d9534f"; Layout.alignment: Qt.AlignHCenter
                    Text {
                        text: "Déconnexion"; color: "white"; font.bold: true; font.pixelSize: menuTextSize; anchors.centerIn: parent
                    }
                    MouseArea {
                        anchors.fill: parent;
                        onClicked: {
                            // NETTOYAGE COMPLET AU LOGOUT
                            clearAllData()
                            stackView.replace(loginPageComponent)
                        }
                    }
                }
            }
        }
    }
}
