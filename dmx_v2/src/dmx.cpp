/**
 * @file dmx.cpp
 * @brief Implémentation de la gestion du protocole DMX512.
 * @details Ce fichier contient le code métier pour interagir avec le 
 * contrôleur matériel DMX de l'ESP32, en utilisant la bibliothèque `esp_dmx`.
 */

#include "dmx.h"
#include <esp_dmx.h>
#include "config.h"

/**
 * @brief Port matériel utilisé pour le DMX.
 * @details L'ESP32 possède plusieurs ports UART. Le port 1 est utilisé 
 * ici pour éviter les conflits avec le port 0 (utilisé par `Serial.print`).
 */
dmx_port_t portDMX = 1;      

/**
 * @brief Chronomètre pour gérer la fréquence d'envoi du signal DMX.
 */
unsigned long dernierEnvoiDMX = 0;

/**
 * @brief Initialise la communication DMX sur l'ESP32.
 * @details Cette fonction installe le driver DMX avec les paramètres 
 * par défaut, assigne les broches de transmission/réception définies dans 
 * `config.h`, et initialise la mémoire tampon avec des valeurs nulles 
 * (pour éteindre tous les projecteurs au démarrage).
 */
void initialiserDMX() {
  dmx_config_t config = DMX_CONFIG_DEFAULT;
  dmx_driver_install(portDMX, &config, NULL, 0);
  
  // Assigne TX_RS232 pour envoyer le signal au module MAX485
  dmx_set_pin(portDMX, TX_RS232, RX_RS232, -1); 
  
  byte donneesVides[513];
  memset(donneesVides, 0, 513);
  dmx_write(portDMX, donneesVides, 513);
}

/**
 * @brief Écrit un tableau de valeurs dans la mémoire DMX de l'ESP32.
 * @param donnees Pointeur vers le tableau contenant les nouvelles valeurs.
 * @param taille La taille du tableau (généralement 513 octets).
 * @details Cette fonction prépare les données, mais ne les envoie pas 
 * physiquement sur le câble. C'est `envoyerSignalDMX()` qui s'en charge.
 */
void ecrireDonneesDMX(uint8_t* donnees, uint16_t taille) {
  dmx_write(portDMX, donnees, taille);
}

/**
 * @brief Déclenche l'émission physique de la trame DMX.
 * @details Utilise la fonction `millis()` pour s'assurer que l'envoi ne se 
 * fait pas plus d'une fois toutes les 25 millisecondes (soit environ 40 fois 
 * par seconde), ce qui correspond au taux de rafraîchissement standard de 
 * l'industrie de l'éclairage.
 */
void envoyerSignalDMX() {
  // Le signal est envoyé 40 fois par seconde vers le projecteur
  if (millis() - dernierEnvoiDMX > 25) {
    dmx_send(portDMX);
    dernierEnvoiDMX = millis();
  }
}