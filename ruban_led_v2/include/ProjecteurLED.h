/**
 * @file ProjecteurLED.h
 * @brief En-tête de la classe ProjecteurLED.
 * @details Ce fichier définit une classe orientée objet permettant de 
 * piloter un ruban de LED (ex: WS2812B) via la bibliothèque FastLED. 
 * Elle inclut la gestion de zones indépendantes et de multiples effets visuels.
 */

#pragma once

#include <Arduino.h>
#include <FastLED.h> 
#include "esp32_snir.h" 

/**
 * @class ProjecteurLED
 * @brief Classe de gestion des effets lumineux sur un ruban LED.
 * @details Encapsule le tableau physique des LEDs et fournit des méthodes 
 * pour allumer, éteindre, flasher (stroboscope) ou animer des portions 
 * (zones) du ruban de manière indépendante.
 */
class ProjecteurLED {
  private:
    /** @brief Tableau mémoire représentant l'état de chaque LED physique. */
    CRGB leds[NB_PIXELS];
    
    /** @brief Tableau mémorisant la couleur de fond fixe pour un maximum de 4 zones. */
    CRGB savedColors[4];
    
    /** @brief Variable de teinte globale (Hue) tournant en boucle pour les effets arc-en-ciel. */
    uint8_t gHue;
    
    /** @brief Chronomètre interne pour cadencer l'effet stroboscopique. */
    unsigned long dernierStrobe;
    
    /** @brief État actuel du stroboscope (vrai = allumé, faux = éteint). */
    bool etatStrobe;

  public:
    /**
     * @brief Constructeur par défaut.
     * @details Initialise les variables internes (timers, teinte de base).
     */
    ProjecteurLED();

    /**
     * @brief Initialise le ruban LED matériel.
     * @details Doit être appelée dans le `setup()`. Configure la broche de sortie, 
     * le type de LED (WS2812B), et initialise le ruban à l'état éteint.
     */
    void initialiser();

    /**
     * @brief Met à jour les variables d'animation internes.
     * @details Doit être appelée régulièrement dans le `loop()`. Elle incrémente 
     * notamment la variable `gHue` pour animer les effets colorés fluides.
     */
    void actualiserHorloge();

    /**
     * @brief Éteint la totalité du ruban LED (Noir).
     * @details Ne force pas l'affichage immédiat, il faut appeler `afficher()` ensuite.
     */
    void toutEteindre();

    /**
     * @brief Pousse les données de la mémoire vers le ruban physique.
     * @details Fait appel à `FastLED.show()`. C'est cette méthode qui allume 
     * véritablement les LEDs avec les couleurs configurées précédemment.
     */
    void afficher();

    /**
     * @brief Règle la luminosité globale du ruban.
     * @param luminosite Valeur DMX de la luminosité (0 = éteint, 255 = maximum).
     */
    void appliquerDimmerGlobal(uint8_t luminosite);

    /**
     * @brief Sauvegarde une couleur de base pour une zone spécifique.
     * @param indexZone Numéro de la zone (0 à 3).
     * @param r Valeur du rouge (0-255).
     * @param g Valeur du vert (0-255).
     * @param b Valeur du bleu (0-255).
     */
    void sauvegarderCouleurZone(int indexZone, uint8_t r, uint8_t g, uint8_t b);

    /**
     * @brief Applique une couleur fixe à un segment précis du ruban.
     * @param debut Index de la première LED de la zone.
     * @param fin Index de la dernière LED de la zone.
     * @param r,g,b Valeurs des couleurs (Rouge, Vert, Bleu).
     */
    void peindreZoneFixe(int debut, int fin, uint8_t r, uint8_t g, uint8_t b);

    /**
     * @brief Restaure les couleurs de base mémorisées sur toutes les zones.
     * @details Utile pour réafficher les couleurs fixes après la fin d'un effet temporaire.
     */
    void peindreToutesZonesMemorisees();

    /**
     * @brief Applique un effet clignotant (Stroboscope) sur une zone.
     * @param debut Index de la première LED.
     * @param fin Index de la dernière LED.
     * @param valeurDMX Vitesse du flash (plus la valeur est haute, plus ça clignote vite).
     */
    void appliquerStrobe(int debut, int fin, uint8_t valeurDMX);

    // ==========================================================
    // MÉTHODES D'EFFETS VISUELS DYNAMIQUES
    // ==========================================================
    // Chaque effet s'applique à un segment spécifique défini par `debut` et `fin`.

    /** @brief Affiche un arc-en-ciel glissant. */
    void effet01_Rainbow(int debut, int fin);
    
    /** @brief Un point lumineux qui rebondit avec une traînée (type K2000/Cylon). */
    void effet02_Chenillard(int debut, int fin);
    
    /** @brief Des pixels de couleurs aléatoires qui s'allument et s'éteignent doucement. */
    void effet03_Confetti(int debut, int fin);
    
    /** @brief Similaire au chenillard mais avec un fondu plus doux et continu. */
    void effet04_Sinelon(int debut, int fin);
    
    /** @brief Des pulsations de couleurs synchronisées sur un rythme (Battements par minute). */
    void effet05_BPM(int debut, int fin);
    
    /** @brief Huit points de couleur qui s'entrecroisent. */
    void effet06_Juggle(int debut, int fin);
    
    /** @brief Clignotement alterné Rouge/Bleu (style gyrophare). */
    void effet07_Police(int debut, int fin);
    
    /** @brief Effet d'accélération lumineuse (type vaisseau spatial). */
    void effet08_WarpDrive(int debut, int fin);
    
    /** @brief Pulsation lente et douce d'une couleur (effet respiration). */
    void effet09_Breathing(int debut, int fin);
    
    /** @brief Simulation organique de flammes (Nécessite souvent une palette de couleurs). */
    void effet10_Feu(int debut, int fin);
};