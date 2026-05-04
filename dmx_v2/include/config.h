/**
 * @file config.h
 * @brief Fichier de configuration globale pour le projet ESP32 Art-Net.
 * * Ce fichier regroupe les paramètres de connexion au réseau (Wi-Fi), 
 * le port de communication pour le protocole Art-Net, ainsi que 
 * la définition des broches matérielles par défaut (I2C).
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include "esp32_snir.h" 

// --- CONFIGURATION WI-FI & ART-NET ---

/**
 * @brief Nom du réseau Wi-Fi (SSID) auquel l'ESP32 doit se connecter.
 */
const char ssid[] = "RouterDMX";    

/**
 * @brief Mot de passe du réseau Wi-Fi cible.
 */
const char password[] = "tititoto";  

/**
 * @brief Port réseau UDP utilisé pour écouter les trames Art-Net.
 * @details Le standard de l'industrie pour le protocole Art-Net est le port 6454.
 */
const int artNetPort = 6454;         

// --- SÉCURITÉ BROCHES ---

#ifndef SDA
  /**
   * @brief Définition de la broche SDA (Data) pour le bus I2C.
   * @details Utilisée principalement pour la communication avec l'écran OLED.
   * Défini sur la broche 21 par défaut si non spécifié ailleurs.
   */
  #define SDA 21
#endif

#ifndef SCL
  /**
   * @brief Définition de la broche SCL (Clock) pour le bus I2C.
   * @details Utilisée principalement pour la communication avec l'écran OLED.
   * Défini sur la broche 22 par défaut si non spécifié ailleurs.
   */
  #define SCL 22
#endif

#endif // CONFIG_H