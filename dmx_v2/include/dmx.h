/**
 * @file dmx.h
 * @brief En-tête pour la gestion du protocole DMX512.
 * * Ce fichier déclare les fonctions nécessaires pour initialiser 
 * le contrôleur matériel DMX de l'ESP32, écrire des données dans 
 * la mémoire tampon (buffer), et envoyer physiquement le signal 
 * sur le bus RS485.
 */

#ifndef DMX_H
#define DMX_H

#include <Arduino.h>

/**
 * @brief Initialise le port DMX de l'ESP32.
 * @details Cette fonction configure le port UART de l'ESP32, assigne les 
 * broches matérielles (TX, RX) et prépare le driver `esp_dmx` pour 
 * l'émission (ou la réception) du signal.
 */
void initialiserDMX();

/**
 * @brief Copie un tableau de valeurs DMX dans le buffer de transmission.
 * * @param donnees Pointeur vers le tableau contenant les valeurs des canaux DMX (de 0 à 255).
 * @param taille Le nombre d'octets à écrire (généralement 513 : 1 octet de Start Code + 512 canaux).
 */
void ecrireDonneesDMX(uint8_t* donnees, uint16_t taille);

/**
 * @brief Déclenche l'envoi physique du signal DMX sur le câble.
 * @details Cette fonction doit être appelée régulièrement dans la boucle 
 * principale (loop) pour rafraîchir les projecteurs. Elle intègre généralement 
 * un délai non bloquant (ex: toutes les 25 ms) pour respecter la norme DMX512.
 */
void envoyerSignalDMX();

#endif // DMX_H