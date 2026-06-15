/**
 * @file PageControle.qml
 * @brief Interface de contrôle DMX en temps réel.
 * @author Valentin
 * @date 2026
 * @details Cette page permet à l'opérateur de sélectionner un univers,
 * d'afficher la liste des équipements et de modifier la valeur de leurs
 * canaux DMX via des curseurs interactifs. Elle intègre des fonctionnalités
 * comme le "Blackout Général" et utilise le C++ (NetworkManager) pour l'envoi
 * asynchrone des données réseau.
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "dmxLogic.js" as DmxLogic

Item {
    id: pageControle

    /**
     * @property int baseFontSize
     * @brief Taille de police de base pour les textes standards.
     */
    property int baseFontSize: 18

    /**
     * @property int titleFontSize
     * @brief Taille de police pour les titres principaux.
     */
    property int titleFontSize: 32

    /**
     * @property int topBarBtnHeight
     * @brief Hauteur standardisée pour les boutons de la barre supérieure.
     */
    property int topBarBtnHeight: 50

    /**
     * @property color commonBtnColor
     * @brief Couleur de fond principale pour les boutons standards.
     */
    property color commonBtnColor: "#2a3449"

    /**
     * @property int mathBtnSize
     * @brief Taille (largeur et hauteur) des boutons d'incrémentation (+ et -).
     */
    property int mathBtnSize: 44

    /**
     * @property int sliderThickness
     * @brief Épaisseur de la barre de fond des curseurs DMX.
     */
    property int sliderThickness: 10

    /**
     * @property int sliderHandleSize
     * @brief Diamètre de la poignée (bouton coulissant) des curseurs DMX.
     */
    property int sliderHandleSize: 28

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        anchors.topMargin: 50
        spacing: 15

        // --- BARRE SUPÉRIEURE ---
        RowLayout {
            Layout.fillWidth: true

            Text { text: "💡 Contrôle"; color: "white"; font.pixelSize: titleFontSize; font.bold: true }

            Item { Layout.fillWidth: true }

            /**
             * @brief Bouton "Blackout Général"
             * @details Déclenche la mise à 0 de tous les canaux DMX via le script JS.
             */
            Rectangle {
                color: "#1a1e29"
                border.color: "#ff3333"
                border.width: 1
                radius: 5
                width: 160
                height: topBarBtnHeight
                Text { text: "Blackout Général"; color: "#ff3333"; font.bold: true; font.pixelSize: baseFontSize; anchors.centerIn: parent }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        // 💡 [EXPLICATION] : On appelle la fonction JavaScript pour traiter tous les projecteurs en boucle.
                        var newMemory = DmxLogic.triggerGlobalBlackout(mainWindow.savedSliderValues, mainWindow.allUniverses, myNetwork)
                        // 💡 [EXPLICATION] : On force l'interface graphique à se rafraichir en remplaçant l'objet mémoire.
                        mainWindow.savedSliderValues = {}
                        mainWindow.savedSliderValues = newMemory
                    }
                }
            }

            /**
             * @brief Sélecteur d'univers DMX
             * @details Filtre la liste des équipements affichés selon l'univers choisi.
             */
            ComboBox {
                id: universeComboBox
                Layout.leftMargin: 10
                Layout.preferredWidth: 250
                Layout.preferredHeight: topBarBtnHeight

                model: ["Tous les univers"].concat(universeNames)

                background: Rectangle { color: commonBtnColor; radius: 5 }
                contentItem: Text {
                    leftPadding: 15; text: universeComboBox.currentText; color: "white"; font.pixelSize: baseFontSize
                    font.bold: true; verticalAlignment: Text.AlignVCenter
                }

                onActivated: function(index) {
                    var targetUniverseIndex = -1;

                    if (index > 0) {
                        targetUniverseIndex = index - 1;
                    }
                }
            }

            Rectangle {
                Layout.leftMargin: 10; color: commonBtnColor; radius: 5; width: 100; height: topBarBtnHeight
                Text { text: "Retour"; color: "white"; font.bold: true; font.pixelSize: baseFontSize; anchors.centerIn: parent }
                // 💡 [EXPLICATION] : .pop() retire la page courante de l'historique et nous ramène au menu.
                MouseArea { anchors.fill: parent; onClicked: pageControle.StackView.view.pop() }
            }
        }

        // --- LISTE DES ÉQUIPEMENTS ---
        ListView {
            id: controlList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 20
            topMargin: 15
            bottomMargin: 40

            // 💡 [EXPLICATION] : Au lieu d'utiliser un ListModel simple, on crée un tableau JS dynamique
            // qui se recalcule à chaque fois qu'on change d'univers dans la ComboBox.
            model: {
                if (!allUniverses || allUniverses.length === 0) return [];
                var listeEquipements = [];

                if (universeComboBox.currentIndex === 0) {
                    for (var i = 0; i < allUniverses.length; i++) {
                        var uid = allUniverses[i].idUnivers !== undefined ? allUniverses[i].idUnivers : 1;
                        var eqs = allUniverses[i].equipements || [];
                        for (var j = 0; j < eqs.length; j++) {
                            eqs[j].parentUnivId = uid;
                            listeEquipements.push(eqs[j]);
                        }
                    }
                } else {
                    var idx = universeComboBox.currentIndex - 1;
                    if (idx >= 0 && idx < allUniverses.length) {
                        var uid2 = allUniverses[idx].idUnivers !== undefined ? allUniverses[idx].idUnivers : 1;
                        var eqs2 = allUniverses[idx].equipements || [];
                        for (var k = 0; k < eqs2.length; k++) {
                            eqs2[k].parentUnivId = uid2;
                            listeEquipements.push(eqs2[k]);
                        }
                    }
                }
                return listeEquipements;
            }

            /**
             * @brief Composant visuel d'un équipement DMX.
             * @details Contient le titre de l'équipement, un bouton Blackout local
             * et un répéteur pour générer ses canaux DMX.
             */
            delegate: Rectangle {
                id: equipmentDelegate
                width: controlList.width
                // 💡 [EXPLICATION] : implicitHeight permet à la carte de grandir automatiquement
                // si le projecteur a beaucoup de canaux.
                height: contentLayout.implicitHeight + 40
                color: "#273147"
                radius: 10

                /**
                 * @property int currentProjId
                 * @brief Identifiant unique de l'équipement courant.
                 */
                // 💡 [EXPLICATION] : Dans un Delegate, "modelData" représente les données de l'équipement en cours de création.
                property int currentProjId: modelData.idEquipement !== undefined ? modelData.idEquipement : (modelData.id !== undefined ? modelData.id : 1)

                /**
                 * @property int currentUnivId
                 * @brief Identifiant de l'univers auquel appartient cet équipement.
                 */
                property int currentUnivId: modelData.parentUnivId !== undefined ? modelData.parentUnivId : 1

                ColumnLayout {
                    id: contentLayout
                    anchors.top: parent.top; anchors.left: parent.left; anchors.right: parent.right
                    anchors.margins: 20; spacing: 15

                    RowLayout {
                        Layout.fillWidth: true
                        Text {
                            text: modelData.nomEquipement !== undefined ? modelData.nomEquipement : (modelData.name !== undefined ? modelData.name : "Equipement")
                            color: "white"; font.bold: true; font.pixelSize: baseFontSize; Layout.fillWidth: true
                        }

                        /**
                         * @brief Bouton "Blackout" spécifique à un équipement.
                         */
                        Rectangle {
                            color: "#1a1e29"; border.color: "#ff3333"; border.width: 1; radius: 5; width: 100; height: 35
                            Text { text: "Blackout"; color: "#ff3333"; font.bold: true; font.pixelSize: baseFontSize; anchors.centerIn: parent }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    var temp = Object.assign({}, mainWindow.savedSliderValues)
                                    var uni = equipmentDelegate.currentUnivId

                                    if (modelData.channels !== undefined) {
                                        for (var i = 0; i < modelData.channels.length; i++) {
                                            var ch = modelData.channels[i]
                                            var numCanal = ch.numeroCanal !== undefined ? ch.numeroCanal : (ch.dmx_address !== undefined ? ch.dmx_address : 1)
                                            var key = uni + "_" + equipmentDelegate.currentProjId + "_" + numCanal

                                            temp[key] = 0
                                            // 💡 [EXPLICATION] : On envoie l'ordre binaire au C++
                                            myNetwork.setDmxValueTCP(uni, equipmentDelegate.currentProjId, numCanal, 0)
                                        }
                                        mainWindow.savedSliderValues = {}
                                        mainWindow.savedSliderValues = temp
                                    }
                                }
                            }
                        }
                    }

                    // --- CANAUX DE L'ÉQUIPEMENT ---
                    // 💡 [EXPLICATION] : Le Repeater est une boucle for visuelle. Il crée une ligne
                    // (RowLayout) pour chaque canal du projecteur courant.
                    Repeater {
                        model: modelData.channels
                        delegate: RowLayout {
                            Layout.fillWidth: true; spacing: 8

                            /**
                             * @property var markersList
                             * @brief Liste des repères visuels calculés d'après les fonctionnalités (valeurMax) du canal.
                             */
                            property var markersList: {
                                var reperes = []
                                if (modelData.fonctionnalites !== undefined && modelData.fonctionnalites.length > 1) {
                                    for (var j = 0; j < modelData.fonctionnalites.length - 1; j++) {
                                        var vMax = modelData.fonctionnalites[j].valeurMax
                                        if (vMax !== undefined) reperes.push(Math.min(255, parseInt(vMax) + 1))
                                    }
                                }
                                return reperes
                            }

                            Rectangle {
                                Layout.preferredWidth: 45; Layout.preferredHeight: 30; radius: 4; color: "#3a4a66"
                                Text {
                                    text: "CH " + (modelData.numeroCanal !== undefined ? modelData.numeroCanal : "?")
                                    color: "white"; font.pixelSize: 14; font.bold: true; anchors.centerIn: parent
                                }
                            }

                            Text {
                                property int sliderVal: Math.round(mySlider.value)
                                text: {
                                    var baseName = (modelData.description !== undefined && modelData.description !== "") ? modelData.description : "Canal"

                                    if (modelData.fonctionnalites !== undefined && modelData.fonctionnalites.length > 0) {
                                        for (var i = 0; i < modelData.fonctionnalites.length; i++) {
                                            var f = modelData.fonctionnalites[i]
                                            var vMin = f.valeurMin !== undefined ? parseInt(f.valeurMin) : 0
                                            var vMax = f.valeurMax !== undefined ? parseInt(f.valeurMax) : 255

                                            if (sliderVal >= vMin && sliderVal <= vMax && f.fonction !== undefined && f.fonction !== "") {
                                                return f.fonction
                                            }
                                        }
                                    }
                                    return baseName
                                }
                                color: "white"; font.pixelSize: baseFontSize; font.bold: false
                                Layout.preferredWidth: 150; Layout.alignment: Qt.AlignVCenter; wrapMode: Text.Wrap
                            }

                            /**
                             * @brief Curseur de contrôle DMX (0-255).
                             * @details Envoie la valeur au NetworkManager à chaque modification
                             * de l'utilisateur, et écoute les modifications réseau pour se mettre à jour.
                             */
                            Slider {
                                id: mySlider
                                Layout.fillWidth: true
                                from: 0; to: 255
                                Layout.alignment: Qt.AlignVCenter

                                /**
                                 * @property bool isUpdatingFromNetwork
                                 * @brief Empêche les boucles infinies de signaux entre QML et C++.
                                 */
                                // 💡 [EXPLICATION] : Ce booléen bloque l'émission réseau si le curseur bouge
                                // TOUT SEUL parce qu'il reçoit un ordre du serveur.
                                property bool isUpdatingFromNetwork: false

                                /**
                                 * @property string sliderKey
                                 * @brief Clé d'identification du canal dans le dictionnaire de sauvegarde.
                                 */
                                property string sliderKey: {
                                    var addr = modelData.numeroCanal !== undefined ? modelData.numeroCanal : 1
                                    var uni = equipmentDelegate.currentUnivId
                                    return uni + "_" + equipmentDelegate.currentProjId + "_" + addr
                                }

                                Component.onCompleted: {
                                    isUpdatingFromNetwork = true
                                    value = mainWindow.savedSliderValues[sliderKey] !== undefined ? mainWindow.savedSliderValues[sliderKey] : 0
                                    isUpdatingFromNetwork = false
                                }

                                Connections {
                                    target: mainWindow
                                    function onSavedSliderValuesChanged() {
                                        // 💡 [EXPLICATION] : (!mySlider.pressed) garantit que le curseur ne sautera pas
                                        // si l'opérateur a le doigt posé dessus en train de faire un réglage.
                                        if (!mySlider.pressed) {
                                            var netVal = mainWindow.savedSliderValues[mySlider.sliderKey]
                                            if (netVal !== undefined && Math.round(mySlider.value) !== netVal) {
                                                mySlider.isUpdatingFromNetwork = true
                                                mySlider.value = netVal
                                                mySlider.isUpdatingFromNetwork = false
                                            }
                                        }
                                    }
                                }

                                onValueChanged: {
                                    if (!isUpdatingFromNetwork) {
                                        var addr = modelData.numeroCanal !== undefined ? modelData.numeroCanal : 1
                                        var uni = equipmentDelegate.currentUnivId

                                        var temp = Object.assign({}, mainWindow.savedSliderValues)
                                        temp[sliderKey] = Math.round(value)
                                        mainWindow.savedSliderValues = temp

                                        // 💡 [EXPLICATION] : Ici on déclenche l'envoi de la trame Fire and Forget C++.
                                        myNetwork.setDmxValueTCP(uni, equipmentDelegate.currentProjId, addr, Math.round(value))
                                    }
                                }

                                onPressedChanged: {
                                    // 💡 [EXPLICATION] : Logique d'aimantation visuelle (Snap to Grid).
                                    if (!pressed && markersList.length > 0) {
                                        var tolerance = 8
                                        for (var i = 0; i < markersList.length; i++) {
                                            if (Math.abs(value - markersList[i]) <= tolerance) {
                                                value = markersList[i]
                                                break
                                            }
                                        }
                                    }
                                }

                                background: Rectangle {
                                    x: parent.leftPadding; y: parent.height / 2 - height / 2
                                    implicitWidth: 200; implicitHeight: sliderThickness
                                    width: parent.availableWidth; height: implicitHeight
                                    radius: sliderThickness / 2; color: "#1c2331"

                                    Rectangle {
                                        width: (sliderHandleSize / 2) + parent.parent.visualPosition * (parent.width - sliderHandleSize)
                                        height: parent.height; radius: sliderThickness / 2; color: "#5d5cff"
                                    }

                                    Repeater {
                                        model: markersList
                                        delegate: Rectangle {
                                            x: (sliderHandleSize / 2) + (modelData / 255.0) * (parent.width - sliderHandleSize) - (width / 2)
                                            y: -4; width: 2; height: parent.height + 8
                                            color: "white"; opacity: 0.8
                                        }
                                    }
                                }
                                handle: Rectangle {
                                    x: parent.leftPadding + parent.visualPosition * (parent.availableWidth - width)
                                    y: parent.topPadding + parent.availableHeight / 2 - height / 2
                                    implicitWidth: sliderHandleSize; implicitHeight: sliderHandleSize
                                    radius: sliderHandleSize / 2; color: "white"; border.color: "#5d5cff"; border.width: 2
                                }
                            }

                            // --- BOUTONS D'INCRÉMENTATION MINUS / PLUS ---
                            Rectangle {
                                Layout.preferredWidth: mathBtnSize; Layout.preferredHeight: mathBtnSize
                                radius: 4; color: commonBtnColor; border.color: "#3a4a66"; border.width: 1
                                Text { text: "-"; color: "white"; font.bold: true; font.pixelSize: baseFontSize; anchors.centerIn: parent }

                                // 💡 [EXPLICATION] : Timer pour baisser la valeur en continu si on reste appuyé.
                                Timer { id: holdMinus; interval: 500; onTriggered: repMinus.start() }
                                Timer { id: repMinus; interval: 30; repeat: true; onTriggered: mySlider.value = Math.max(0, mySlider.value - 1) }

                                MouseArea {
                                    anchors.fill: parent
                                    // 💡 [EXPLICATION] : Math.max(0, ...) empêche de descendre sous 0.
                                    onPressed: { mySlider.value = Math.max(0, mySlider.value - 1); holdMinus.start() }
                                    onReleased: { holdMinus.stop(); repMinus.stop() }
                                    onCanceled: { holdMinus.stop(); repMinus.stop() }
                                }
                            }

                            Rectangle {
                                Layout.preferredWidth: mathBtnSize; Layout.preferredHeight: mathBtnSize
                                radius: 4; color: commonBtnColor; border.color: "#3a4a66"; border.width: 1
                                Text { text: "+"; color: "white"; font.bold: true; font.pixelSize: baseFontSize; anchors.centerIn: parent }

                                Timer { id: holdPlus; interval: 500; onTriggered: repPlus.start() }
                                Timer { id: repPlus; interval: 30; repeat: true; onTriggered: mySlider.value = Math.min(255, mySlider.value + 1) }

                                MouseArea {
                                    anchors.fill: parent
                                    // 💡 [EXPLICATION] : Math.min(255, ...) empêche de dépasser la valeur DMX max.
                                    onPressed: { mySlider.value = Math.min(255, mySlider.value + 1); holdPlus.start() }
                                    onReleased: { holdPlus.stop(); repPlus.stop() }
                                    onCanceled: { holdPlus.stop(); repPlus.stop() }
                                }
                            }

                            Text {
                                text: Math.round(mySlider.value); color: "white"; font.pixelSize: baseFontSize
                                font.bold: true; Layout.preferredWidth: 40; horizontalAlignment: Text.AlignRight; Layout.alignment: Qt.AlignVCenter
                            }
                        }
                    }
                }
            }
        }
    }
}
