/**
 * @file EcranOLED.h
 * @brief En-tête de la classe EcranOLED.
 * @details Ce fichier définit une classe personnalisée qui encapsule 
 * la gestion de l'écran OLED en héritant de la bibliothèque standard SSD1306.
 */

#pragma once

#include <Arduino.h> 
#include <SSD1306.h> 

/**
 * @class EcranOLED
 * @brief Classe de gestion de l'affichage OLED pour le contrôleur DMX.
 * @details Hérite de la classe SSD1306 pour étendre ses fonctionnalités avec 
 * des méthodes spécifiques au projet (affichage des zones, effets, et trames).
 */
class EcranOLED : public SSD1306 {
  private:
    /**
     * @brief Mémorise le moment (en millisecondes) du dernier affichage.
     * @details Utilisé en interne pour ne pas saturer le bus I2C et éviter 
     * le scintillement de l'écran en limitant la fréquence de rafraîchissement.
     */
    unsigned long dernierRafraichissement;

  public:
    /**
     * @brief Constructeur de la classe EcranOLED.
     * @param address Adresse I2C de l'écran (ex: 0x3C ou ADD_OLED).
     * @param sda Broche de données I2C (ex: 21).
     * @param scl Broche d'horloge I2C (ex: 22).
     */
    EcranOLED(uint8_t address, uint8_t sda, uint8_t scl);

    /**
     * @brief Initialise physiquement l'écran.
     * @details Doit être appelée une seule fois dans le `setup()`. Elle 
     * démarre la communication I2C, retourne l'image si besoin et efface l'écran.
     */
    void initialiser();

    /**
     * @brief Rafraîchit l'écran avec les données de fonctionnement en temps réel.
     * @param nomZone Le nom de la zone DMX actuellement contrôlée (ex: "Zone 1").
     * @param nomEffet Le nom de l'effet d'animation en cours (ex: "Chenillard").
     * @param infoExtra Une information additionnelle ou paramètre de l'effet.
     * @param tramesRecues Le nombre total de trames Art-Net reçues.
     * @details Cette méthode intègre son propre minuteur basé sur `dernierRafraichissement` 
     * pour ne se déclencher qu'à intervalle régulier (ex: toutes les 500ms).
     */
    void actualiser(String nomZone, String nomEffet, String infoExtra, long tramesRecues);

    /**
     * @brief Affiche un message ponctuel plein écran.
     * @param titre Le titre du message (ex: "ATTENTE").
     * @param valeur Le contenu du message (ex: "Connexion Wi-Fi...").
     * @details Utile pour les états de transition comme le démarrage, 
     * la perte de réseau ou l'affichage de l'adresse IP.
     */
    void afficherMessage(String titre, String valeur); 
};