/**
 * @file ProjecteurLED.cpp
 * @brief Implémentation de la classe ProjecteurLED.
 * @details Ce fichier contient le code métier pour l'animation et le contrôle 
 * matériel d'un ruban LED adressable (WS2812B) via la bibliothèque FastLED.
 */

#include "ProjecteurLED.h"

/**
 * @brief Constructeur de la classe.
 * @details Initialise les variables d'animation (teinte à 0, timers à 0) 
 * et remplit le tableau des couleurs sauvegardées avec du noir (éteint).
 */
ProjecteurLED::ProjecteurLED() {
  gHue = 0;
  dernierStrobe = 0;
  etatStrobe = false;
  for (int i = 0; i < 4; i++) {
    savedColors[i] = CRGB::Black;
  }
}

/**
 * @brief Configuration matérielle initiale du ruban LED.
 * @details Indique à FastLED le type de puce (WS2812B), la broche de données, 
 * l'ordre des couleurs (GRB), et applique une correction colorimétrique standard.
 */
void ProjecteurLED::initialiser() {
  FastLED.addLeds<WS2812B, DATALEDS, GRB>(leds, NB_PIXELS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(255);
  FastLED.clear();
  FastLED.show();
}

/**
 * @brief Met à jour l'horloge interne d'animation.
 * @details Utilise la macro non bloquante `EVERY_N_MILLISECONDS` de FastLED 
 * pour faire doucement tourner la teinte de base (`gHue`) toutes les 20ms.
 */
void ProjecteurLED::actualiserHorloge() {
  EVERY_N_MILLISECONDS(20) { gHue++; }
}

/**
 * @brief Éteint logiciellement toutes les LEDs.
 */
void ProjecteurLED::toutEteindre() { 
  FastLED.clear(); 
}

/**
 * @brief Rafraîchit l'affichage matériel.
 */
void ProjecteurLED::afficher() { 
  FastLED.show(); 
}

/**
 * @brief Applique une baisse de luminosité sur l'ensemble du ruban.
 * @param luminosite Valeur d'intensité entre 0 (noir) et 255 (max).
 * @details Utilise `nscale8` pour assombrir proportionnellement la couleur 
 * actuelle de chaque pixel sans perdre la proportion RGB d'origine.
 */
void ProjecteurLED::appliquerDimmerGlobal(uint8_t luminosite) {
  for (int i = 0; i < NB_PIXELS; i++) {
    leds[i].nscale8(luminosite);
  }
}

/**
 * @brief Mémorise une couleur pour une des 4 zones du ruban.
 * @param indexZone Numéro de la mémoire (0 à 3).
 * @param r,g,b Code couleur.
 */
void ProjecteurLED::sauvegarderCouleurZone(int indexZone, uint8_t r, uint8_t g, uint8_t b) {
  if (indexZone >= 0 && indexZone < 4) {
    savedColors[indexZone] = CRGB(r, g, b);
  }
}

/**
 * @brief Applique une couleur unie sur un segment précis du ruban.
 * @param debut Premier pixel du segment.
 * @param fin Dernier pixel du segment (exclu).
 * @param r,g,b Code couleur.
 */
void ProjecteurLED::peindreZoneFixe(int debut, int fin, uint8_t r, uint8_t g, uint8_t b) {
  for (int i = debut; i < fin; i++) {
    leds[i] = CRGB(r, g, b);
  }
}

/**
 * @brief Restitue le découpage en 4 zones avec leurs couleurs mémorisées.
 * @details Les découpages sont fixes : 0-14, 15-29, 30-44, 45-59.
 */
void ProjecteurLED::peindreToutesZonesMemorisees() {
  for (int i = 0; i < 15; i++)  { leds[i] = savedColors[0]; }
  for (int i = 15; i < 30; i++) { leds[i] = savedColors[1]; }
  for (int i = 30; i < 45; i++) { leds[i] = savedColors[2]; }
  for (int i = 45; i < 60; i++) { leds[i] = savedColors[3]; }
}

/**
 * @brief Superpose un effet clignotant sur une zone.
 * @param debut Premier pixel de la zone.
 * @param fin Dernier pixel.
 * @param valeurDMX La valeur brute DMX (de 150 à 249) qui détermine la vitesse.
 * @details Convertit la plage DMX en un délai d'alternance (entre 500ms et 25ms).
 */
void ProjecteurLED::appliquerStrobe(int debut, int fin, uint8_t valeurDMX) {
  int vitesseStrobe = map(valeurDMX, 150, 249, 500, 25);
  
  if (millis() - dernierStrobe > vitesseStrobe) {
    dernierStrobe = millis();
    etatStrobe = !etatStrobe;
  }
  
  // Si le stroboscope est dans son cycle "éteint", on écrase la couleur avec du noir
  if (!etatStrobe) {
    for (int i = debut; i < fin; i++) {
      leds[i] = CRGB::Black;
    }
  }
}

// ==========================================================
// FONCTIONS D'EFFETS DYNAMIQUES
// ==========================================================

/**
 * @brief Remplissage arc-en-ciel glissant.
 */
void ProjecteurLED::effet01_Rainbow(int debut, int fin) { 
  fill_rainbow(&leds[debut], fin - debut, gHue, 7); 
}

/**
 * @brief Ligne pointillée qui avance dans une direction.
 */
void ProjecteurLED::effet02_Chenillard(int debut, int fin) { 
  int nbLeds = fin - debut;
  fill_solid(&leds[debut], nbLeds, CRGB::Black);
  uint8_t offset = (millis() / 80) % 3; 
  
  for (int i = offset; i < nbLeds; i += 3) { 
    leds[debut + i] = CHSV(gHue - (i * 5), 255, 255); 
  } 
}

/**
 * @brief Étincelles aléatoires qui scintillent puis s'effacent.
 */
void ProjecteurLED::effet03_Confetti(int debut, int fin) { 
  int nbLeds = fin - debut; 
  fadeToBlackBy(&leds[debut], nbLeds, 10); 
  leds[debut + random16(nbLeds)] += CHSV(gHue + random8(64), 200, 255); 
}

/**
 * @brief Point lumineux qui fait des va-et-vient avec une traînée (Cylon).
 */
void ProjecteurLED::effet04_Sinelon(int debut, int fin) { 
  int nbLeds = fin - debut; 
  fadeToBlackBy(&leds[debut], nbLeds, 20); 
  int pos = beatsin16(13, 0, nbLeds - 1); 
  leds[debut + pos] += CHSV(gHue, 255, 192); 
}

/**
 * @brief Pulsation colorée rythmée.
 */
void ProjecteurLED::effet05_BPM(int debut, int fin) { 
  uint8_t beat = beatsin8(62, 64, 255); 
  
  for (int i = debut; i < fin; i++) {
    leds[i] = ColorFromPalette(RainbowColors_p, gHue + (i * 2), beat - gHue + (i * 10)); 
  }
}

/**
 * @brief Multiples points lumineux qui s'entrecroisent.
 */
void ProjecteurLED::effet06_Juggle(int debut, int fin) { 
  int nbLeds = fin - debut; 
  fadeToBlackBy(&leds[debut], nbLeds, 20); 
  uint8_t dothue = 0; 
  
  for (int i = 0; i < 8; i++) { 
    leds[debut + beatsin16(i + 7, 0, nbLeds - 1)] |= CHSV(dothue, 200, 255); 
    dothue += 32; 
  } 
}

/**
 * @brief Alternance rapide façon gyrophare de police.
 */
void ProjecteurLED::effet07_Police(int debut, int fin) { 
  uint8_t beat = beat8(150); 
  
  if (beat < 128) {
    fill_solid(&leds[debut], fin - debut, CRGB::Red);
  } else {
    fill_solid(&leds[debut], fin - debut, CRGB::Blue);
  }
}

/**
 * @brief Effet d'expansion depuis le centre.
 */
void ProjecteurLED::effet08_WarpDrive(int debut, int fin) { 
  int nbLeds = fin - debut; 
  int centre = nbLeds / 2; 
  
  for (int i = 0; i <= centre; i++) { 
    uint8_t onde = sin8((i * 20) - (millis() / 4)); 
    uint8_t luminosite = 0;
    
    if (onde > 127) {
      luminosite = map(onde, 127, 255, 0, 255);
    }
    
    CRGB couleur = CHSV(gHue - (i * 3), 255, luminosite); 
    
    if (centre + i < nbLeds) {
      leds[debut + centre + i] = couleur;
    }
    if (centre - i >= 0) {
      leds[debut + centre - i] = couleur; 
    }
  } 
}

/**
 * @brief Oscillation lente de la luminosité d'une couleur.
 */
void ProjecteurLED::effet09_Breathing(int debut, int fin) { 
  uint8_t breath = beatsin8(30, 0, 255); 
  fill_solid(&leds[debut], fin - debut, CHSV(gHue, 255, breath)); 
}

/**
 * @brief Génération procédurale de flammes.
 */
void ProjecteurLED::effet10_Feu(int debut, int fin) { 
  for (int i = 0; i < (fin - debut); i++) { 
    uint8_t heat = inoise8(i * 40, millis() / 4); 
    leds[debut + i] = ColorFromPalette(HeatColors_p, heat, 255); 
  } 
}