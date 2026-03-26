#include <Arduino.h>
#include <esp_dmx.h>
#include <SSD1306.h>
#include <Adafruit_NeoPixel.h>

// On inclut ton fichier matériel pour avoir les bonnes broches (DATALEDS, etc.)
#define MODELE_A 
#include "esp32_snir.h" 


// ==========================================================
// CONFIGURATION RUBAN LED (Adafruit NeoPixel)
// ==========================================================
// NB_PIXELS et DATALEDS sont fournis par esp32_snir.h
Adafruit_NeoPixel pixels(NB_PIXELS, DATALEDS, NEO_RGB + NEO_KHZ800);

// Découpage en 5 zones (15 canaux au total pour 1 Scanner Starway)
#define NUM_ZONES 5
int ledsParZone = NB_PIXELS / NUM_ZONES; 

// ==========================================================
// CONFIGURATION DMX (RÉCEPTEUR)
// ==========================================================
#define DMX_START_ADDRESS 1   
dmx_port_t dmxPort = 1;       
#define RX_PIN 16             // Broche de réception DMX (MAX485 RO)
byte dmxData[DMX_PACKET_SIZE]; 

// ==========================================================
// ÉCRAN OLED
// ==========================================================
SSD1306Wire afficheurOLED(ADD_OLED, SDA, SCL);
unsigned long dernierRafraichissementOLED = 0;
long tramesRecues = 0; 

void setup() {
  Serial.begin(115200);

  // 1. ÉCRAN OLED
  afficheurOLED.init();
  afficheurOLED.flipScreenVertically();
  
  // 2. RUBAN LED
  pixels.begin();
  pixels.setBrightness(255); // Le DMX gère l'intensité de chaque couleur
  pixels.clear();
  pixels.show();

  // 3. PORT DMX
  dmx_config_t config = DMX_CONFIG_DEFAULT;
  dmx_driver_install(dmxPort, &config, NULL, 0); 
  // RX_PIN (16) est en écoute, TX et EN désactivés (-1)
  dmx_set_pin(dmxPort, -1, RX_PIN, -1);  
}

void loop() {
  dmx_packet_t packet;
  
  // ==========================================================
  // 1. ÉCOUTE DU DMX
  // ==========================================================
  if (dmx_receive(dmxPort, &packet, 1)) {
    if (!packet.err) {
      dmx_read(dmxPort, dmxData, packet.size);
      tramesRecues++;

      // ==========================================================
      // 2. MAPPAGE DMX -> LEDS
      // ==========================================================
      for (int zone = 0; zone < NUM_ZONES; zone++) {
        // dmxData[0] est le Start Code. Le canal 1 est dans dmxData[1].
        int dmxIndex = DMX_START_ADDRESS + (zone * 3);

        if (dmxIndex + 2 < packet.size) {
          uint8_t r = dmxData[dmxIndex];     // Canal 1 (Rouge Zone 1)
          uint8_t g = dmxData[dmxIndex + 1]; // Canal 2 (Vert Zone 1)
          uint8_t b = dmxData[dmxIndex + 2]; // Canal 3 (Bleu Zone 1)

          // On allume les LEDs de cette zone
          for (int i = 0; i < ledsParZone; i++) {
            int indexLed = (zone * ledsParZone) + i;
            pixels.setPixelColor(indexLed, r, g, b);
          }
        }
      }
      
      // On met à jour le ruban physique
      pixels.show();
    }
  }

  // ==========================================================
  // 3. AFFICHAGE ÉCRAN (2 fois par seconde)
  // ==========================================================
  if (millis() - dernierRafraichissementOLED > 500) {
    afficheurOLED.clear();
    
    afficheurOLED.setFont(ArialMT_Plain_16);
    afficheurOLED.drawString(0, 0, "Mode : Starway");
    
    afficheurOLED.setFont(ArialMT_Plain_10);
    afficheurOLED.drawString(0, 25, "Adresse DMX : " + String(DMX_START_ADDRESS));
    afficheurOLED.drawString(0, 40, "DMX Rx : " + String(tramesRecues));
    
    afficheurOLED.display();
    dernierRafraichissementOLED = millis();
  }
}