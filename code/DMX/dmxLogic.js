/**
 * @file dmxLogic.js
 * @brief Logique métier et manipulation des données pour l'interface QML.
 * @author Valentin
 * @date 2026
 * @details Contient les fonctions utilitaires pour formater les données reçues,
 * gérer la mémoire locale des valeurs DMX et exécuter des actions globales
 * sans surcharger le code QML.
 */

// 💡 [EXPLICATION] : .pragma library est une directive spéciale de QML.
// Elle indique que ce fichier JS doit être partagé entre toutes les pages (comme un Singleton).
// Sans ça, chaque page créerait sa propre copie du script, ce qui gaspillerait de la mémoire.
.pragma library

/**
 * @brief Extrait et formate les noms des univers à partir des données brutes.
 * @param {Array} data Tableau d'objets contenant les informations des univers.
 * @return {Array} Un tableau de chaînes de caractères (noms des univers).
 */
function formatUniverseNames(data) {
    var names = [];
    for (var i = 0; i < data.length; i++) {
        // 💡 [EXPLICATION] : Si la base de données n'a pas renvoyé de nom (undefined),
        // on crée un nom par défaut "Univers X" pour éviter un affichage vide dans la ComboBox.
        var nom = data[i].nom_univers !== undefined ? data[i].nom_univers : "Univers " + (i + 1);
        names.push(nom);
    }
    return names;
}

/**
 * @brief Met à jour la mémoire locale avec les nouvelles valeurs d'une scène.
 * @details Préserve les valeurs des univers qui ne sont pas affectés par la nouvelle scène
 * pour éviter les coupures de lumière non désirées.
 * @param {Object} currentMemory Dictionnaire actuel des valeurs DMX (clé: "idUniv_idProj_idCanal").
 * @param {Array} valuesList Tableau des nouvelles valeurs DMX à appliquer.
 * @return {Object} Le nouveau dictionnaire de mémoire mis à jour.
 */
function processSceneValues(currentMemory, valuesList) {
    var nextMemory = {};
    var affectedUniverses = {};

    // 💡 [EXPLICATION] : Étape 1 - On dresse la liste des univers que la scène va modifier.
    for (var i = 0; i < valuesList.length; i++) {
        affectedUniverses[valuesList[i].idUnivers.toString()] = true;
    }

    var keys = Object.keys(currentMemory);
    // 💡 [EXPLICATION] : Étape 2 - On copie l'état actuel des curseurs pour les univers NON affectés.
    // C'est vital pour le métier : lancer une scène sur l'Univers 2 ne doit pas éteindre l'Univers 1 !
    for (var k = 0; k < keys.length; k++) {
        var keyName = keys[k];
        var univPrefix = keyName.split("_")[0];
        if (!affectedUniverses[univPrefix]) {
            nextMemory[keyName] = currentMemory[keyName];
        }
    }

    // 💡 [EXPLICATION] : Étape 3 - On écrase ou on ajoute les nouvelles valeurs de la scène.
    for (var j = 0; j < valuesList.length; j++) {
        var item = valuesList[j];
        // 💡 [EXPLICATION] : On génère la clé unique (ex: "1_12_5" = Univers 1, Proj 12, Canal 5)
        var newKey = item.idUnivers + "_" + item.idEquipement + "_" + item.numeroCanal;
        nextMemory[newKey] = item.valeurCanaux;
    }
    return nextMemory;
}

/**
 * @brief Construit la mémoire locale initiale à partir de l'arborescence complète.
 * @param {Object} currentMemory Dictionnaire actuel (souvent vide au démarrage).
 * @param {Array} allUniverses L'arborescence complète (Univers > Equipements > Canaux).
 * @return {Object} Le dictionnaire contenant toutes les valeurs en direct extraites.
 */
function extractLiveValues(currentMemory, allUniverses) {
    // 💡 [EXPLICATION] : Object.assign permet de cloner l'objet mémoire.
    // En QML, pour qu'un dictionnaire déclenche le rafraîchissement visuel de l'écran,
    // il faut remplacer l'ancien dictionnaire par un nouveau.
    var newMemory = Object.assign({}, currentMemory);

    // 💡 [EXPLICATION] : Triple boucle imbriquée pour parcourir l'arbre JSON (Univers -> Équipements -> Canaux).
    for (var i = 0; i < allUniverses.length; i++) {
        var uni = allUniverses[i];
        var idUni = uni.idUnivers !== undefined ? uni.idUnivers : (uni.id !== undefined ? uni.id : 1);

        if (uni.equipements !== undefined) {
            for (var j = 0; j < uni.equipements.length; j++) {
                var equip = uni.equipements[j];
                var idProj = equip.idEquipement !== undefined ? equip.idEquipement : (equip.id !== undefined ? equip.id : 1);

                if (equip.channels !== undefined) {
                    for (var k = 0; k < equip.channels.length; k++) {
                        var ch = equip.channels[k];
                        var numCanal = ch.numeroCanal !== undefined ? ch.numeroCanal : (ch.dmx_address !== undefined ? ch.dmx_address : 1);

                        // 💡 [EXPLICATION] : Le nom de la variable JSON peut changer selon la réponse du serveur
                        // (valeur, valeurActuelle, valeurCanaux). On teste les 3 possibilités pour que l'app soit robuste.
                        var val = ch.valeur !== undefined ? ch.valeur :
                                 (ch.valeurActuelle !== undefined ? ch.valeurActuelle :
                                 (ch.valeurCanaux !== undefined ? ch.valeurCanaux : undefined));

                        if (val !== undefined) {
                            newMemory[idUni + "_" + idProj + "_" + numCanal] = val;
                        }
                    }
                }
            }
        }
    }
    return newMemory;
}

/**
 * @brief Déclenche un Blackout global sur l'ensemble des projecteurs connus.
 * @details Parcourt toute l'arborescence, envoie une commande TCP à 0 pour chaque canal,
 * et met à jour la mémoire locale visuelle.
 * @param {Object} currentMemory Dictionnaire actuel des valeurs.
 * @param {Array} allUniverses Arborescence de tous les univers et équipements.
 * @param {Object} networkManager Instance du C++ NetworkManager pour l'envoi TCP.
 * @return {Object} Le nouveau dictionnaire avec toutes les valeurs à 0.
 */
function triggerGlobalBlackout(currentMemory, allUniverses, networkManager) {
    var newMemory = Object.assign({}, currentMemory);

    // 💡 [EXPLICATION] : On repasse dans toute l'arborescence JSON connue.
    for (var i = 0; i < allUniverses.length; i++) {
        var uni = allUniverses[i];
        var idUni = uni.idUnivers !== undefined ? uni.idUnivers : (uni.id !== undefined ? uni.id : 1);

        if (uni.equipements !== undefined) {
            for (var j = 0; j < uni.equipements.length; j++) {
                var equip = uni.equipements[j];
                var idProj = equip.idEquipement !== undefined ? equip.idEquipement : (equip.id !== undefined ? equip.id : 1);

                if (equip.channels !== undefined) {
                    for (var k = 0; k < equip.channels.length; k++) {
                        var ch = equip.channels[k];
                        var numCanal = ch.numeroCanal !== undefined ? ch.numeroCanal : (ch.dmx_address !== undefined ? ch.dmx_address : 1);

                        // 💡 [EXPLICATION] : C'est ici que la magie opère ! Depuis le Javascript,
                        // on appelle la fonction C++ (Q_INVOKABLE) pour forcer le matériel à s'éteindre en temps réel.
                        networkManager.setDmxValueTCP(idUni, idProj, numCanal, 0);

                        // 💡 [EXPLICATION] : Et on force la mémoire visuelle (les curseurs) à descendre à 0.
                        newMemory[idUni + "_" + idProj + "_" + numCanal] = 0;
                    }
                }
            }
        }
    }
    return newMemory;
}
