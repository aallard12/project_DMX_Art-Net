/* 
 * File:   esp32_snir.h
 * Author: philippe
 *
 * Created on 4 janvier 2024, 08:31
 */

#ifndef ESP32_SNIR_H
#define ESP32_SNIR_H

#define BP1 39 // définition des boutons-poussoirs
#define BP2 34
#define BP3 35

#define SW 23 // définition de l’interrupteur

#define LED 2 // définition de la LED bleue sur le board ESP32
#define D1 13 // définition de la LED rouge
#define D2 12 // définition de la LED verte

#define ADD_OLED 0x3C // adresse de l’afficheur OLED sur le bus I2C

#define DATALEDS 19 // définition du GPIO pour la commande des LED RGB
#define NB_PIXELS 4 // nombre de pixels sur la carte.

#define ADC_LDR 36 // définition du GPIO pour la conversion analogique numérique

#define RELAY_1 27 // définition du GPIO pour le relais 1
#define RELAY_2 14 // définition du GPIO pour le relais 2

#define TX_RS232 17 // définition de la broche de transmission pour la RS232
#define RX_RS232 16 // définition de la broche de réception pour la RS232

#define TEMP 18 // définition du bus 1-wire pour le capteur de température

#define PWM 2 // définition du GPIO pour le servomoteur

// définition pour le clavier suivant le modèle A ou B
// à définir MODELE_A ou MODELE_B avec un #define ou 
// ajouter dans le fichier platformio.ini  : build_flags = -DMODELE_A ou build_flags = -DMODELE_B
#if defined(MODELE_A)
// Lignes
#define L0 32
#define L1 33
#define L2 25
#define L3 26
// Colonnes
#define C0 4
#define C1 5
#define C2 15

#elif defined(MODELE_B)
// Lignes
#define L0 26
#define L1 25
#define L2 33
#define L3 32
// Colonnes
#define C0 15
#define C1 5
#define C2 4

#else

// Aucun clavier défini : utilisation facultative
// (aucune erreur, aucune broche déclarée)

#endif /* choix du modèle de clavier */

#endif /* ESP32_SNIR_H */

