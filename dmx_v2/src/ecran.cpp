/**
 * @file ecran.cpp
 * @brief Implémentation des fonctions d'affichage OLED.
 * @details Ce fichier contient le code permettant d'interagir avec 
 * l'écran OLED connecté en I2C (via la bibliothèque SSD1306Wire), 
 * pour afficher les statuts de connexion et les statistiques réseau.
 */

#include "ecran.h"
#include <SSD1306.h>
#include "config.h"

/**
 * @brief Instance globale de l'afficheur OLED.
 * @details Les paramètres de broches (ADD_OLED, SDA, SCL) sont fournis 
 * par le fichier `config.h` (ou `esp32_snir.h`).
 */
SSD1306Wire afficheurOLED(ADD_OLED, SDA, SCL);

/**
 * @brief Chronomètre pour gérer la fréquence de mise à jour de l'écran.
 */
unsigned long dernierRafraichissementOLED = 0;

/**
 * @brief Prépare l'écran OLED lors du démarrage.
 * @details Initialise le composant, inverse l'orientation de l'affichage 
 * (utile selon comment l'écran est soudé sur la maquette) et efface l'écran.
 */
void initialiserEcran() {
  afficheurOLED.init();
  afficheurOLED.flipScreenVertically();
  afficheurOLED.clear();
}

/**
 * @brief Affiche le message de recherche réseau.
 * @details Dessine le texte "Connexion Wi-Fi..." au centre de l'écran 
 * pendant que l'ESP32 cherche à se connecter au routeur.
 */
void afficherAttenteWifi() {
  afficheurOLED.clear();
  afficheurOLED.setFont(ArialMT_Plain_16);
  afficheurOLED.drawString(10, 20, "Connexion Wi-Fi...");
  afficheurOLED.display();
}

/**
 * @brief Affiche le résultat de la tentative de connexion Wi-Fi.
 * @param connecte État de la connexion (réussie ou échouée).
 * @param ip L'adresse IP locale à afficher en cas de succès.
 * @details En cas de succès, affiche un grand "OK" et l'adresse IP. 
 * En cas d'échec, affiche un grand "NO WIFI".
 */
void afficherStatutWifi(bool connecte, String ip) {
  afficheurOLED.clear();
  
  if (connecte) {
    afficheurOLED.setFont(ArialMT_Plain_24);
    afficheurOLED.drawString(45, 0, "OK");
    afficheurOLED.setFont(ArialMT_Plain_10);
    afficheurOLED.drawString(10, 30, "IP: " + ip);
  } else {
    afficheurOLED.setFont(ArialMT_Plain_24);
    afficheurOLED.drawString(30, 0, "NO WIFI");
  }
  
  afficheurOLED.display();
}

/**
 * @brief Met à jour le compteur de trames sur l'écran.
 * @param paquetsRecus Le nombre de paquets Art-Net reçus.
 * @details Cette fonction est conçue pour ne se déclencher qu'une fois 
 * par seconde (grâce à `millis()`). Au lieu d'effacer tout l'écran 
 * (ce qui provoquerait un clignotement désagréable), elle ne dessine 
 * qu'un rectangle noir sur la zone du texte pour écraser l'ancien chiffre 
 * avant d'écrire le nouveau.
 */
void rafraichirEcran(long paquetsRecus) {
  if (millis() - dernierRafraichissementOLED > 1000) {
    // Efface uniquement la zone basse de l'écran
    afficheurOLED.setColor(BLACK);
    afficheurOLED.fillRect(0, 45, 128, 19); 
    
    // Réécrit le texte en blanc
    afficheurOLED.setColor(WHITE);
    afficheurOLED.setFont(ArialMT_Plain_10);
    afficheurOLED.drawString(10, 45, "Art-Net Rx: " + String(paquetsRecus));
    
    // Envoie l'image modifiée à l'écran
    afficheurOLED.display();
    
    dernierRafraichissementOLED = millis();
  }
}