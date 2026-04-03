#include "dmx.h"
#include <esp_dmx.h>
#include "config.h"

dmx_port_t portDMX = 1;      
unsigned long dernierEnvoiDMX = 0;

void initialiserDMX() {
  dmx_config_t config = DMX_CONFIG_DEFAULT;
  dmx_driver_install(portDMX, &config, NULL, 0);
  
  // On assigne TX_RS232 pour envoyer le signal au MAX485
  dmx_set_pin(portDMX, TX_RS232, RX_RS232, -1); 
  
  byte donneesVides[513];
  memset(donneesVides, 0, 513);
  dmx_write(portDMX, donneesVides, 513);
}

void ecrireDonneesDMX(uint8_t* donnees, uint16_t taille) {
  dmx_write(portDMX, donnees, taille);
}

void envoyerSignalDMX() {
  // Le signal est envoyé 40 fois par seconde vers le projecteur
  if (millis() - dernierEnvoiDMX > 25) {
    dmx_send(portDMX);
    dernierEnvoiDMX = millis();
  }
}