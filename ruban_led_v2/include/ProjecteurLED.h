#pragma once

#include <Arduino.h>
#include <FastLED.h> // Uniquement FastLED, pas besoin du reste ici !

// On inclut ton fichier matériel ici pour que la classe connaisse NB_PIXELS et DATALEDS
#define MODELE_A 
#include "esp32_snir.h" 

// ===================================================================================
// CLASSE 1 : GESTION DU RUBAN LED
// ===================================================================================
class ProjecteurLED {
  private:
    CRGB leds[NB_PIXELS];
    CRGB savedColors[4];
    uint8_t gHue;
    unsigned long dernierStrobe;
    bool etatStrobe;

  public:
    ProjecteurLED();
    void initialiser();
    void actualiserHorloge();
    void toutEteindre();
    void afficher();
    void appliquerDimmerGlobal(uint8_t luminosite);
    void sauvegarderCouleurZone(int indexZone, uint8_t r, uint8_t g, uint8_t b);
    void peindreZoneFixe(int debut, int fin, uint8_t r, uint8_t g, uint8_t b);
    void peindreToutesZonesMemorisees();
    void appliquerStrobe(int debut, int fin, uint8_t valeurDMX);

    // Effets (Avec le nouveau Chenillard en 2)
    void effet01_Rainbow(int debut, int fin);
    void effet02_Chenillard(int debut, int fin);
    void effet03_Confetti(int debut, int fin);
    void effet04_Sinelon(int debut, int fin);
    void effet05_BPM(int debut, int fin);
    void effet06_Juggle(int debut, int fin);
    void effet07_Police(int debut, int fin);
    void effet08_WarpDrive(int debut, int fin);
    void effet09_Breathing(int debut, int fin);
    void effet10_Feu(int debut, int fin);
};