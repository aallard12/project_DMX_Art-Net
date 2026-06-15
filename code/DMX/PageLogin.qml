import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

/**
 * @file PageLogin.qml
 * @brief Page d'authentification de l'application Tablette DMX.
 * @author Valentin
 * @date 2026
 * @details Interface permettant à l'utilisateur de s'identifier avant d'accéder
 * au panneau de contrôle. Affiche l'état de la connexion TCP et gère les
 * retours d'authentification (succès ou échec) envoyés par le serveur.
 */

Item {
    id: pageLogin
    objectName: "pageLogin"

    /**
     * @signal loginSuccess
     * @brief Émis lorsque le serveur confirme la validité des identifiants.
     * @details Ce signal est généralement capté par le composant parent (Main.qml)
     * pour déclencher la navigation vers les pages principales (Scènes, Contrôle).
     */
    // 💡 [EXPLICATION] : Signal maison. Quand les identifiants sont bons, on l'émet,
    // et Main.qml fait le changement de page.
    signal loginSuccess()

    /**
     * @property bool loginFailed
     * @brief Indique si la dernière tentative de connexion a échoué.
     * @details Utilisé pour déclencher l'affichage du message d'erreur et
     * colorer les bordures des champs de saisie en rouge.
     */
    property bool loginFailed: false

    /**
     * @brief Écoute des signaux émis par le backend C++ (NetworkManager).
     * @details Intercepte la réponse du serveur après une tentative de login
     * pour mettre à jour l'interface en conséquence.
     */
    Connections {
        target: myNetwork
        // 💡 [EXPLICATION] : Le C++ nous répond ici. Si success est Vrai, on vide les champs (sécurité).
        function onLoginResponseReceived(success) {
            if (success) {
                pageLogin.loginFailed = false
                pseudoField.text = ""
                mdpField.text = ""
                pageLogin.loginSuccess()
            } else {
                pageLogin.loginFailed = true // Déclenche l'erreur rouge
            }
        }
    }

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 20

        // --- TITRE ---
        Text {
            text: "Tablette DMX"
            color: "white"
            font.pixelSize: 42
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
            Layout.bottomMargin: 5
        }

        /**
         * @brief Pastille d'état de la connexion serveur.
         * @details Change de couleur et de texte selon la propriété `isConnected` du NetworkManager.
         */
        Rectangle {
            Layout.preferredWidth: 200
            Layout.preferredHeight: 40
            radius: 20
            Layout.alignment: Qt.AlignHCenter
            Layout.bottomMargin: 10
            color: myNetwork.isConnected ? "#1E3A1E" : "#3A1E1E"
            border.color: myNetwork.isConnected ? "#00a86b" : "#ff3333"
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
                    text: myNetwork.isConnected ? "CONNECTÉ" : "DÉCONNECTÉ"
                    color: parent.parent.border.color
                    font.bold: true; font.pixelSize: 14
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }

        /**
         * @brief Message d'erreur dynamique.
         * @details N'apparaît que si la propriété `loginFailed` est à true.
         */
        Text {
            text: "❌ Identifiant ou mot de passe incorrect"
            color: "#ff3333"
            font.pixelSize: 16
            font.bold: true
            visible: pageLogin.loginFailed // 💡 [EXPLICATION] : Apparaît et disparaît dynamiquement.
            Layout.alignment: Qt.AlignHCenter
        }

        // --- CHAMPS DE SAISIE ---
        RowLayout {
            spacing: 15
            Text { text: "Identifiant :"; color: "white"; font.pixelSize: 20; Layout.preferredWidth: 150 }
            TextField {
                id: pseudoField
                font.pixelSize: 20
                Layout.preferredWidth: 250
                color: "white"
                background: Rectangle {
                    color: "#2a3449"; radius: 5;
                    border.color: pageLogin.loginFailed ? "#ff3333" : "#5d5cff";
                    border.width: 1
                }
                onTextChanged: pageLogin.loginFailed = false
            }
        }

        RowLayout {
            spacing: 15
            Text { text: "Mot de passe :"; color: "white"; font.pixelSize: 20; Layout.preferredWidth: 150 }
            TextField {
                id: mdpField
                // 💡 [EXPLICATION] : Masque le mot de passe visuellement.
                echoMode: TextInput.Password
                font.pixelSize: 20
                Layout.preferredWidth: 250
                color: "white"
                background: Rectangle {
                    color: "#2a3449"; radius: 5;
                    border.color: pageLogin.loginFailed ? "#ff3333" : "#5d5cff";
                    border.width: 1
                }
                onTextChanged: pageLogin.loginFailed = false
            }
        }

        // --- BOUTONS D'ACTION ---

        /**
         * @brief Bouton de soumission du formulaire.
         * @details Est désactivé visuellement et fonctionnellement si le serveur
         * n'est pas joignable (pas de connexion TCP). Envoie les identifiants en clair
         * au NetworkManager lors du clic.
         */
        Rectangle {
            Layout.preferredWidth: 250
            Layout.preferredHeight: 60
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: 10
            radius: 10
            color: myNetwork.isConnected ? "#00a86b" : "#555555"

            Text {
                text: "Se Connecter"
                color: "white"
                font.bold: true
                font.pixelSize: 20
                anchors.centerIn: parent
            }

            MouseArea {
                anchors.fill: parent
                // 💡 [EXPLICATION] : Rend le bouton non-cliquable si le Pi n'est pas trouvé sur le réseau.
                enabled: myNetwork.isConnected
                onClicked: {
                    if (pseudoField.text !== "" && mdpField.text !== "") {
                        // Envoi au backend C++
                        myNetwork.sendLoginTCP(pseudoField.text, mdpField.text)
                    }
                }
            }
        }

        /**
         * @brief Bouton pour fermer l'application.
         */
        Rectangle {
            Layout.preferredWidth: 250
            Layout.preferredHeight: 60
            Layout.alignment: Qt.AlignHCenter
            radius: 10
            color: "#d9534f"

            Text { text: "Quitter"; color: "white"; font.bold: true; font.pixelSize: 20; anchors.centerIn: parent }

            MouseArea {
                anchors.fill: parent
                onClicked: Qt.quit() // 💡 [EXPLICATION] : Fonction de base de Qt pour fermer le programme.
            }
        }
    }
}
