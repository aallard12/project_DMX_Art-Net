/**
 * @file EcranOLED.cpp
 * @brief Implémentation de la classe EcranOLED.
 * @details Ce fichier contient le code des méthodes de la classe 
 * personnalisée gérant l'afficheur I2C basé sur le contrôleur SSD1306.
 */

#include <Arduino.h>   
#include "EcranOLED.h"

/**
 * @brief Constructeur de la classe.
 * @param address L'adresse I2C du périphérique.
 * @param sda La broche de données.
 * @param scl La broche d'horloge.
 * @details Initialise la classe parente `SSD1306` et remet le chronomètre 
 * interne de rafraîchissement à zéro.
 */
EcranOLED::EcranOLED(uint8_t address, uint8_t sda, uint8_t scl) : SSD1306(address, sda, scl) {
  dernierRafraichissement = 0;
}

/**
 * @brief Initialise physiquement l'écran.
 * @details Appelle les méthodes de la classe parente pour démarrer 
 * l'affichage et l'orienter correctement. Doit être placée dans `setup()`.
 */
void EcranOLED::initialiser() {
  init(); 
  flipScreenVertically(); 
}

/**
 * @brief Met à jour l'affichage avec les données courantes.
 * @param nomZone Le texte descriptif de la zone sélectionnée.
 * @param nomEffet Le texte descriptif de l'effet en cours de lecture.
 * @param infoExtra Une variable supplémentaire (ex: Vitesse, Couleur).
 * @param tramesRecues Le compteur de paquets DMX/Art-Net.
 * @details Ne met à jour l'écran physique qu'une fois toutes les 500ms 
 * afin de ne pas bloquer le microcontrôleur. Efface l'écran et redessine 
 * quatre lignes d'informations.
 */
void EcranOLED::actualiser(String nomZone, String nomEffet, String infoExtra, long tramesRecues) {
  if (millis() - dernierRafraichissement > 500) {
    clear(); 
    setFont(ArialMT_Plain_10);
    drawString(0, 0, "DMX: OOP Architecture");
    drawString(0, 15, "Z : " + nomZone);
    drawString(0, 27, "FX: " + nomEffet + " " + infoExtra);
    drawString(0, 40, "Rx: " + String(tramesRecues));
    display(); 
    dernierRafraichissement = millis();
  }
}

/**
 * @brief Affiche un message d'information simple.
 * @param titre Ligne supérieure du message.
 * @param valeur Ligne inférieure du message.
 * @details Utile pour les alertes ou les phases d'initialisation 
 * (par exemple : affichage de l'adresse IP).
 */
void EcranOLED::afficherMessage(String titre, String valeur) {
  clear(); 
  setFont(ArialMT_Plain_10);
  drawString(0, 10, titre);
  drawString(0, 30, valeur);
  display(); 
}