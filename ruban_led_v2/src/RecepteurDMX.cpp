/**
 * @file RecepteurDMX.cpp
 * @brief Implémentation de la classe RecepteurDMX.
 * @details Ce fichier contient le code métier responsable de la configuration 
 * matérielle du port UART de l'ESP32 et de la récupération des données 
 * issues du module MAX485.
 */

#include "RecepteurDMX.h"

/**
 * @brief Constructeur de la classe RecepteurDMX.
 * @details Initialise le port UART utilisé (1 par défaut pour éviter 
 * les conflits avec le Serial) et met le compteur de trames reçues à zéro.
 */
RecepteurDMX::RecepteurDMX() {
  dmxPort = 1;
  tramesRecues = 0;
}

/**
 * @brief Configure le matériel pour la réception.
 * @param pinRx Le numéro de la broche connectée à la sortie du MAX485.
 * @details Installe le driver `esp_dmx` avec les paramètres par défaut et 
 * assigne la broche de réception. Les broches TX et EN sont désactivées (`-1`).
 */
void RecepteurDMX::initialiser(int pinRx) {
  dmx_config_t config = DMX_CONFIG_DEFAULT;
  dmx_driver_install(dmxPort, &config, NULL, 0);
  dmx_set_pin(dmxPort, -1, pinRx, -1);
}

/**
 * @brief Vérifie et récupère les données DMX entrantes.
 * @return `true` si une trame valide a été lue, `false` sinon.
 * @details Utilise un timeout de `0` pour ne jamais bloquer le programme 
 * (lecture non-bloquante). Si une trame sans erreur est présente dans le 
 * buffer UART, elle est copiée dans le tableau `dmxData` et le compteur 
 * global est incrémenté.
 */
bool RecepteurDMX::ecouter() {
  dmx_packet_t packet;
  // dmx_receive avec un timeout de 0 : on regarde juste s'il y a quelque chose
  if (dmx_receive(dmxPort, &packet, 0)) {
    // On s'assure qu'il n'y a pas eu de micro-coupure ou d'erreur électrique
    if (!packet.err) {
      // Transfert des données du matériel vers notre mémoire vive (RAM)
      dmx_read(dmxPort, dmxData, packet.size);
      tramesRecues++;
      return true;
    }
  }
  return false;
}

/**
 * @brief Lit de manière sécurisée la valeur d'un canal DMX.
 * @param canal Le numéro de canal DMX (de 1 à 512).
 * @return La valeur du canal (0 à 255), ou 0 si le canal demandé n'existe pas.
 * @details Intègre une vérification des limites (`canal >= 1 && canal < 513`) 
 * pour éviter les plantages (Segmentation Fault) si on demande un canal invalide.
 */
uint8_t RecepteurDMX::lireCanal(int canal) {
  if (canal >= 1 && canal < 513) return dmxData[canal];
  return 0; // Valeur par défaut en cas d'erreur
}

/**
 * @brief Récupère le nombre de paquets traités.
 * @return Le compteur de trames reçues depuis le démarrage de l'ESP32.
 */
long RecepteurDMX::getTramesRecues() {
  return tramesRecues;
}