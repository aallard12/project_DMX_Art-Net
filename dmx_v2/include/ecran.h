/**
 * @file ecran.h
 * @brief En-tête pour la gestion de l'affichage sur l'écran OLED.
 * @details Ce fichier déclare les fonctions permettant d'initialiser 
 * l'afficheur, de montrer l'état de la connexion réseau (Wi-Fi) et de 
 * mettre à jour les compteurs de réception de trames en temps réel.
 */

#ifndef ECRAN_H
#define ECRAN_H

#include <Arduino.h>

/**
 * @brief Initialise l'écran OLED.
 * @details Configure les paramètres de communication I2C, retourne l'écran 
 * (flip) si nécessaire, et efface la mémoire d'affichage pour préparer le démarrage.
 */
void initialiserEcran();

/**
 * @brief Affiche un écran de chargement pour le réseau.
 * @details Utilisée exclusivement dans le `setup()` pour faire patienter 
 * l'utilisateur pendant que l'ESP32 tente de se connecter au routeur Wi-Fi.
 */
void afficherAttenteWifi();

/**
 * @brief Affiche le bilan de la tentative de connexion Wi-Fi.
 * @param connecte Booléen indiquant si l'ESP32 est bien connecté au réseau (`true`) ou non (`false`).
 * @param ip L'adresse IP locale attribuée par le routeur sous forme de chaîne de caractères.
 * @details Si la connexion a réussi, affiche "OK" et l'adresse IP (indispensable pour 
 * configurer le serveur Art-Net). Si elle échoue, affiche une erreur "NO WIFI".
 */
void afficherStatutWifi(bool connecte, String ip);

/**
 * @brief Rafraîchit les données dynamiques sur l'écran.
 * @param paquetsRecus Le nombre total de trames Art-Net ou DMX traitées depuis le démarrage.
 * @details Cette fonction est appelée en boucle dans le `loop()`. Elle intègre 
 * un système de minuterie (généralement 500ms ou 1000ms) pour ne pas redessiner 
 * l'écran à chaque tour de boucle, ce qui ferait ralentir ou planter l'ESP32.
 */
void rafraichirEcran(long paquetsRecus);

#endif // ECRAN_H