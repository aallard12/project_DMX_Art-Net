#include "ProjecteurLED.h"


ProjecteurLED::ProjecteurLED() {
  gHue = 0;
  dernierStrobe = 0;
  etatStrobe = false;
  for (int i = 0; i < 4; i++) savedColors[i] = CRGB::Black;
}

void ProjecteurLED::initialiser() {
  FastLED.addLeds<WS2812B, DATALEDS, GRB>(leds, NB_PIXELS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(255);
  FastLED.clear();
  FastLED.show();
}

void ProjecteurLED::actualiserHorloge() {
  EVERY_N_MILLISECONDS(20) { gHue++; }
}

void ProjecteurLED::toutEteindre() { FastLED.clear(); }
void ProjecteurLED::afficher() { FastLED.show(); }

void ProjecteurLED::appliquerDimmerGlobal(uint8_t luminosite) {
  for (int i = 0; i < NB_PIXELS; i++) leds[i].nscale8(luminosite);
}

void ProjecteurLED::sauvegarderCouleurZone(int indexZone, uint8_t r, uint8_t g, uint8_t b) {
  if (indexZone >= 0 && indexZone < 4) savedColors[indexZone] = CRGB(r, g, b);
}

void ProjecteurLED::peindreZoneFixe(int debut, int fin, uint8_t r, uint8_t g, uint8_t b) {
  for (int i = debut; i < fin; i++) leds[i] = CRGB(r, g, b);
}

void ProjecteurLED::peindreToutesZonesMemorisees() {
  for (int i = 0; i < 15; i++)  leds[i] = savedColors[0];
  for (int i = 15; i < 30; i++) leds[i] = savedColors[1];
  for (int i = 30; i < 45; i++) leds[i] = savedColors[2];
  for (int i = 45; i < 60; i++) leds[i] = savedColors[3];
}

void ProjecteurLED::appliquerStrobe(int debut, int fin, uint8_t valeurDMX) {
  int vitesseStrobe = map(valeurDMX, 150, 249, 500, 25);
  if (millis() - dernierStrobe > vitesseStrobe) {
    dernierStrobe = millis();
    etatStrobe = !etatStrobe;
  }
  if (!etatStrobe) {
    for (int i = debut; i < fin; i++) leds[i] = CRGB::Black;
  }
}

// -- Effets --
void ProjecteurLED::effet01_Rainbow(int debut, int fin) { fill_rainbow(&leds[debut], fin - debut, gHue, 7); }

void ProjecteurLED::effet02_Chenillard(int debut, int fin) { 
  int nbLeds = fin - debut;
  fill_solid(&leds[debut], nbLeds, CRGB::Black);
  uint8_t offset = (millis() / 80) % 3; 
  for (int i = offset; i < nbLeds; i += 3) { 
    leds[debut + i] = CHSV(gHue - (i * 5), 255, 255); 
  } 
}

void ProjecteurLED::effet03_Confetti(int debut, int fin) { int nbLeds = fin - debut; fadeToBlackBy(&leds[debut], nbLeds, 10); leds[debut + random16(nbLeds)] += CHSV(gHue + random8(64), 200, 255); }
void ProjecteurLED::effet04_Sinelon(int debut, int fin) { int nbLeds = fin - debut; fadeToBlackBy(&leds[debut], nbLeds, 20); int pos = beatsin16(13, 0, nbLeds - 1); leds[debut + pos] += CHSV(gHue, 255, 192); }
void ProjecteurLED::effet05_BPM(int debut, int fin) { uint8_t beat = beatsin8(62, 64, 255); for (int i = debut; i < fin; i++) leds[i] = ColorFromPalette(RainbowColors_p, gHue + (i * 2), beat - gHue + (i * 10)); }
void ProjecteurLED::effet06_Juggle(int debut, int fin) { int nbLeds = fin - debut; fadeToBlackBy(&leds[debut], nbLeds, 20); uint8_t dothue = 0; for (int i = 0; i < 8; i++) { leds[debut + beatsin16(i + 7, 0, nbLeds - 1)] |= CHSV(dothue, 200, 255); dothue += 32; } }
void ProjecteurLED::effet07_Police(int debut, int fin) { uint8_t beat = beat8(150); fill_solid(&leds[debut], fin - debut, (beat < 128) ? CRGB::Red : CRGB::Blue); }
void ProjecteurLED::effet08_WarpDrive(int debut, int fin) { int nbLeds = fin - debut; int centre = nbLeds / 2; for (int i = 0; i <= centre; i++) { uint8_t onde = sin8((i * 20) - (millis() / 4)); uint8_t luminosite = (onde > 127) ? map(onde, 127, 255, 0, 255) : 0; CRGB couleur = CHSV(gHue - (i * 3), 255, luminosite); if (centre + i < nbLeds) leds[debut + centre + i] = couleur; if (centre - i >= 0) leds[debut + centre - i] = couleur; } }
void ProjecteurLED::effet09_Breathing(int debut, int fin) { uint8_t breath = beatsin8(30, 0, 255); fill_solid(&leds[debut], fin - debut, CHSV(gHue, 255, breath)); }
void ProjecteurLED::effet10_Feu(int debut, int fin) { for (int i = 0; i < (fin - debut); i++) { uint8_t heat = inoise8(i * 40, millis() / 4); leds[debut + i] = ColorFromPalette(HeatColors_p, heat, 255); } }