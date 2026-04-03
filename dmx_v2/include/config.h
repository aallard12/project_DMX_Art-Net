#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include "esp32_snir.h" // Fournit TX_RS232, RX_RS232, ADD_OLED, SDA, SCL

// --- CONFIGURATION WI-FI & ART-NET ---
const char ssid[] = "RouterDMX";     // Le nom de ton réseau Wi-Fi
const char password[] = "tititoto";  // Le mot de passe
const int artNetPort = 6454;         // Port de réception UDP officiel

// --- SÉCURITÉ BROCHES ---
#ifndef SDA
  #define SDA 21
#endif
#ifndef SCL
  #define SCL 22
#endif

#endif