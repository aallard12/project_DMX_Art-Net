#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include "esp32_snir.h" 

// --- CONFIGURATION WI-FI & ART-NET ---
const char ssid[] = "RouterDMX";    
const char password[] = "tititoto";  
const int artNetPort = 6454;         

// --- SÉCURITÉ BROCHES ---
#ifndef SDA
  #define SDA 21
#endif
#ifndef SCL
  #define SCL 22
#endif

#endif