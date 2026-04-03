#pragma once

#include <Arduino.h>   // Pour les types byte, uint8_t, etc.
#include <esp_dmx.h>   // Indispensable pour dmx_port_t et DMX_PACKET_SIZE

// ===================================================================================
// CLASSE 3 : GESTION DU DMX
// ===================================================================================
class RecepteurDMX {
  private:
    dmx_port_t dmxPort;
    byte dmxData[DMX_PACKET_SIZE];
    long tramesRecues;

  public:
    RecepteurDMX();
    void initialiser(int pinRx);
    bool ecouter();
    uint8_t lireCanal(int canal);
    long getTramesRecues();
};