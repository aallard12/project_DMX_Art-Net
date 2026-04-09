#pragma once

#include <Arduino.h>  
#include <esp_dmx.h> 


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