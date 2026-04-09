#pragma once

#include <Arduino.h> 
#include <SSD1306.h> 


class EcranOLED : public SSD1306 {
  private:
    unsigned long dernierRafraichissement;

  public:
    EcranOLED(uint8_t address, uint8_t sda, uint8_t scl);
    void initialiser();
    void actualiser(String nomZone, String nomEffet, String infoExtra, long tramesRecues);
    void afficherMessage(String titre, String valeur); 
};