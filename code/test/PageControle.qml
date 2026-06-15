/**
 * @file PageControle.qml
 * @brief Page de contrôle manuel des équipements DMX.
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: pageControle

    /** @brief Signal déclenché pour mettre tous les curseurs locaux à 0. */
    signal globalBlackout()

    /** @name Paramètres de Design UI */
    property int baseFontSize: 18
    property int titleFontSize: 32
    property int topBarBtnHeight: 50
    property color commonBtnColor: "#2a3449"
    property int mathBtnSize: 44
    property int sliderThickness: 10
    property int sliderHandleSize: 28

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        anchors.topMargin: 50
        spacing: 15

        RowLayout {
            Layout.fillWidth: true

            Text {
                text: "💡 Contrôle"
                color: "white"
                font.pixelSize: titleFontSize
                font.bold: true
            }

            Item { Layout.fillWidth: true }

            Rectangle {
                color: "#1a1e29"
                border.color: "#ff3333"
                border.width: 1
                radius: 5
                width: 160
                height: topBarBtnHeight
                Text {
                    text: "Blackout Général"
                    color: "#ff3333"
                    font.bold: true
                    font.pixelSize: baseFontSize
                    anchors.centerIn: parent
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        mainWindow.savedSliderValues = ({})
                        pageControle.globalBlackout()
                    }
                }
            }

            Rectangle {
                Layout.leftMargin: 10
                color: commonBtnColor
                radius: 5
                width: 120
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
                    onClicked: myNetwork.fetchUniverses()
                }
            }

            ComboBox {
                id: universeComboBox
                Layout.leftMargin: 10
                Layout.preferredWidth: 250
                Layout.preferredHeight: topBarBtnHeight
                model: universeNames
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
                    onClicked: pageControle.StackView.view.pop()
                }
            }
        }

        ListView {
            id: controlList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 20
            topMargin: 15
            bottomMargin: 40

            model: (universeComboBox.currentIndex >= 0 && allUniverses.length > 0) ? allUniverses[universeComboBox.currentIndex].equipements : []

            delegate: Rectangle {
                id: equipmentDelegate
                width: controlList.width
                height: contentLayout.implicitHeight + 40
                color: "#273147"
                radius: 10

                property int currentProjId: modelData.idEquipement !== undefined ? modelData.idEquipement : (modelData.id !== undefined ? modelData.id : 1)

                signal blackoutFixture()

                Connections {
                    target: pageControle
                    function onGlobalBlackout() {
                        equipmentDelegate.blackoutFixture()
                    }
                }

                ColumnLayout {
                    id: contentLayout
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.margins: 20
                    spacing: 15

                    RowLayout {
                        Layout.fillWidth: true

                        Text {
                            text: modelData.name !== undefined ? modelData.name : (modelData.nomEquipement !== undefined ? modelData.nomEquipement : "Equipement")
                            color: "white"
                            font.bold: true
                            font.pixelSize: baseFontSize
                            Layout.fillWidth: true
                        }

                        Rectangle {
                            color: "#1a1e29"
                            border.color: "#ff3333"
                            border.width: 1
                            radius: 5
                            width: 100
                            height: 35
                            Text {
                                text: "Blackout"
                                color: "#ff3333"
                                font.bold: true
                                font.pixelSize: baseFontSize
                                anchors.centerIn: parent
                            }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: equipmentDelegate.blackoutFixture()
                            }
                        }
                    }

                    Repeater {
                        model: modelData.channels
                        delegate: RowLayout {
                            Layout.fillWidth: true
                            spacing: 8

                            Connections {
                                target: equipmentDelegate
                                function onBlackoutFixture() {
                                    mySlider.value = 0
                                    var addr = modelData.dmx_address !== undefined ? modelData.dmx_address : (modelData.numeroCanal !== undefined ? modelData.numeroCanal : 1)
                                    var uni = (universeComboBox.currentIndex >= 0 && allUniverses.length > 0) ? (allUniverses[universeComboBox.currentIndex].idUnivers !== undefined ? allUniverses[universeComboBox.currentIndex].idUnivers : 1) : 1
                                    myNetwork.setDmxValueTCP(uni, equipmentDelegate.currentProjId, addr, 0)
                                }
                            }

                            // --- 🎯 LOGIQUE DES RÉFÉRENCES BDD ---
                            property var markersList: {
                                var reperes = []
                                // On utilise 'valeurMax' de la table FONCTIONNALITE_CANAL
                                if (modelData.fonctionnalites !== undefined && modelData.fonctionnalites.length > 1) {
                                    for (var j = 0; j < modelData.fonctionnalites.length - 1; j++) {
                                        if (modelData.fonctionnalites[j].valeurMax !== undefined) {
                                            reperes.push(modelData.fonctionnalites[j].valeurMax)
                                        }
                                    }
                                }
                                return reperes
                            }

                            property bool isMultiFunction: modelData.fonctionnalites !== undefined && modelData.fonctionnalites.length > 1

                            // --- 📝 NOM DU CANAL DYNAMIQUE ---
                            Text {
                                text: {
                                    var val = Math.round(mySlider.value)
                                    var baseName = modelData.description !== undefined ? modelData.description : (modelData.chName !== undefined ? modelData.chName : "Canal")

                                    if (!isMultiFunction) return baseName

                                    // Recherche de la fonction active selon la plage [valeurMin, valeurMax]
                                    for (var i = 0; i < modelData.fonctionnalites.length; i++) {
                                        var f = modelData.fonctionnalites[i]
                                        var vMin = f.valeurMin !== undefined ? f.valeurMin : 0
                                        var vMax = f.valeurMax !== undefined ? f.valeurMax : 255
                                        if (val >= vMin && val <= vMax) {
                                            return f.fonction !== undefined ? f.fonction : baseName
                                        }
                                    }
                                    return baseName
                                }
                                color: isMultiFunction ? "#ffb74d" : "white" // Orange pour les multi-fonctions
                                font.pixelSize: baseFontSize
                                font.bold: isMultiFunction
                                Layout.preferredWidth: 150
                                Layout.alignment: Qt.AlignVCenter
                                wrapMode: Text.Wrap
                            }

                            Slider {
                                id: mySlider
                                Layout.fillWidth: true
                                from: 0
                                to: 255
                                Layout.alignment: Qt.AlignVCenter

                                property string sliderKey: {
                                    var addr = modelData.dmx_address !== undefined ? modelData.dmx_address : (modelData.numeroCanal !== undefined ? modelData.numeroCanal : 1)
                                    var uni = (universeComboBox.currentIndex >= 0 && allUniverses.length > 0) ? (allUniverses[universeComboBox.currentIndex].idUnivers !== undefined ? allUniverses[universeComboBox.currentIndex].idUnivers : 1) : 1
                                    return uni + "_" + equipmentDelegate.currentProjId + "_" + addr
                                }

                                value: mainWindow.savedSliderValues[sliderKey] !== undefined ? mainWindow.savedSliderValues[sliderKey] : (modelData.initVal !== undefined ? modelData.initVal : 0)

                                onValueChanged: {
                                    var addr = modelData.dmx_address !== undefined ? modelData.dmx_address : (modelData.numeroCanal !== undefined ? modelData.numeroCanal : 1)
                                    var uni = (universeComboBox.currentIndex >= 0 && allUniverses.length > 0) ? (allUniverses[universeComboBox.currentIndex].idUnivers !== undefined ? allUniverses[universeComboBox.currentIndex].idUnivers : 1) : 1

                                    var temp = mainWindow.savedSliderValues
                                    temp[sliderKey] = Math.round(value)
                                    mainWindow.savedSliderValues = temp

                                    myNetwork.setDmxValueTCP(uni, equipmentDelegate.currentProjId, addr, Math.round(value))
                                }

                                // Magnétisme (Snapping) sur les séparateurs
                                onPressedChanged: {
                                    if (!pressed && isMultiFunction) {
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
                                    x: parent.leftPadding
                                    y: parent.height / 2 - height / 2
                                    implicitWidth: 200
                                    implicitHeight: sliderThickness
                                    width: parent.availableWidth
                                    height: implicitHeight
                                    radius: sliderThickness / 2
                                    color: "#1c2331"

                                    Rectangle {
                                        width: parent.parent.visualPosition * parent.width
                                        height: parent.height
                                        radius: sliderThickness / 2
                                        color: isMultiFunction ? "#ffb74d" : "#5d5cff"
                                    }

                                    // --- 🚧 BARRES DE SÉPARATION VISUELLES ---
                                    Repeater {
                                        model: markersList
                                        delegate: Rectangle {
                                            x: (modelData / 255.0) * parent.width - (width / 2)
                                            y: -4
                                            width: 2
                                            height: parent.height + 8
                                            color: "white"
                                            opacity: 0.8
                                        }
                                    }
                                }
                                handle: Rectangle {
                                    x: parent.leftPadding + parent.visualPosition * (parent.availableWidth - width)
                                    y: parent.topPadding + parent.availableHeight / 2 - height / 2
                                    implicitWidth: sliderHandleSize
                                    implicitHeight: sliderHandleSize
                                    radius: sliderHandleSize / 2
                                    color: "white"
                                    border.color: isMultiFunction ? "#ffb74d" : "#5d5cff"
                                    border.width: 2
                                }
                            }

                            Rectangle {
                                Layout.preferredWidth: mathBtnSize
                                Layout.preferredHeight: mathBtnSize
                                radius: 4
                                color: commonBtnColor
                                border.color: "#3a4a66"
                                border.width: 1
                                Text {
                                    text: "-"
                                    color: "white"
                                    font.bold: true
                                    font.pixelSize: baseFontSize
                                    anchors.centerIn: parent
                                }
                                Timer { id: holdMinus; interval: 500; onTriggered: repMinus.start() }
                                Timer { id: repMinus; interval: 30; repeat: true; onTriggered: mySlider.value = Math.max(0, mySlider.value - 1) }
                                MouseArea {
                                    anchors.fill: parent
                                    onPressed: { mySlider.value = Math.max(0, mySlider.value - 1); holdMinus.start() }
                                    onReleased: { holdMinus.stop(); repMinus.stop() }
                                    onCanceled: { holdMinus.stop(); repMinus.stop() }
                                }
                            }

                            Rectangle {
                                Layout.preferredWidth: mathBtnSize
                                Layout.preferredHeight: mathBtnSize
                                radius: 4
                                color: commonBtnColor
                                border.color: "#3a4a66"
                                border.width: 1
                                Text {
                                    text: "+"
                                    color: "white"
                                    font.bold: true
                                    font.pixelSize: baseFontSize
                                    anchors.centerIn: parent
                                }
                                Timer { id: holdPlus; interval: 500; onTriggered: repPlus.start() }
                                Timer { id: repPlus; interval: 30; repeat: true; onTriggered: mySlider.value = Math.min(255, mySlider.value + 1) }
                                MouseArea {
                                    anchors.fill: parent
                                    onPressed: { mySlider.value = Math.min(255, mySlider.value + 1); holdPlus.start() }
                                    onReleased: { holdPlus.stop(); repPlus.stop() }
                                    onCanceled: { holdPlus.stop(); repPlus.stop() }
                                }
                            }

                            Text {
                                text: Math.round(mySlider.value)
                                color: "white"
                                font.pixelSize: baseFontSize
                                font.bold: true
                                Layout.preferredWidth: 40
                                horizontalAlignment: Text.AlignRight
                                Layout.alignment: Qt.AlignVCenter
                            }
                        }
                    }
                }
            }
        }
    }
}
