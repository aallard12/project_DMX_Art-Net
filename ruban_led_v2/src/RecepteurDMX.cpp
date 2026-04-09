#include "RecepteurDMX.h"


RecepteurDMX::RecepteurDMX() {
  dmxPort = 1;
  tramesRecues = 0;
}

void RecepteurDMX::initialiser(int pinRx) {
  dmx_config_t config = DMX_CONFIG_DEFAULT;
  dmx_driver_install(dmxPort, &config, NULL, 0);
  dmx_set_pin(dmxPort, -1, pinRx, -1);
}

bool RecepteurDMX::ecouter() {
  dmx_packet_t packet;
  if (dmx_receive(dmxPort, &packet, 0)) {
    if (!packet.err) {
      dmx_read(dmxPort, dmxData, packet.size);
      tramesRecues++;
      return true;
    }
  }
  return false;
}

uint8_t RecepteurDMX::lireCanal(int canal) {
  if (canal >= 1 && canal < 513) return dmxData[canal];
  return 0;
}

long RecepteurDMX::getTramesRecues() {
  return tramesRecues;
}