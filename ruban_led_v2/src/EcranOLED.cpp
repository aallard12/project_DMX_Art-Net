#include <Arduino.h>   
#include "EcranOLED.h"


EcranOLED::EcranOLED(uint8_t address, uint8_t sda, uint8_t scl) : SSD1306(address, sda, scl) {
  dernierRafraichissement = 0;
}

void EcranOLED::initialiser() {
  init(); 
  flipScreenVertically(); 
}

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


void EcranOLED::afficherMessage(String titre, String valeur) {
  clear(); 
  setFont(ArialMT_Plain_10);
  drawString(0, 10, titre);
  drawString(0, 30, valeur);
  display(); 
}