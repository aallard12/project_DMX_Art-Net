#pragma once

#include <Arduino.h> // Indispensable pour String et uint8_t
#include <SSD1306.h> // Indispensable pour hériter de la classe SSD1306

// ===================================================================================
// CLASSE 2 : GESTION DE L'ÉCRAN OLED (Héritage de SSD1306)
// ===================================================================================
class EcranOLED : public SSD1306 {
  private:
    unsigned long dernierRafraichissement;

  public:
    EcranOLED(uint8_t address, uint8_t sda, uint8_t scl);
    void initialiser();
    void actualiser(String nomZone, String nomEffet, String infoExtra, long tramesRecues);
};