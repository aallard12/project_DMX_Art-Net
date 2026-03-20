#include <Arduino.h>
#include "esp32_snir.h"
#include <SSD1306.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Keypad.h>
#include <Adafruit_NeoPixel.h>
#include <esp_dmx.h>
#include <ArtnetWifi.h>
#include <WiFi.h>

// ==========================================================
// INTERRUPTEUR DE TEST : Mets sur 'false' le jour de la vraie mise en service !
#define MODE_TEST true 
// ==========================================================


// --- CONFIGURATION WI-FI & ART-NET ---
const char* ssid = "RouterDMX";
const char* password = "tititoto";

WiFiUDP udp;
const int artNetPort = 6454; 
uint8_t packetBuffer[530];   

// --- CONFIGURATION DMX ---
dmx_port_t dmxPort = 1;      
byte dmxData[513];           
unsigned long dernierEnvoiDMX = 0;

// --- INITIALISATION ÉCRAN ---
SSD1306Wire afficheurOLED(ADD_OLED, SDA, SCL);
unsigned long dernierRafraichissementOLED = 0;
long paquetsRecus = 0;

void setup() {
  Serial.begin(115200);

  // 1. DÉMARRAGE ÉCRAN OLED
  afficheurOLED.init();
  afficheurOLED.flipScreenVertically();
  afficheurOLED.clear();
  afficheurOLED.setFont(ArialMT_Plain_16);
  afficheurOLED.drawString(10, 20, "Connexion Wi-Fi...");
  afficheurOLED.display();

  // 2. CONFIGURATION DU PORT DMX (esp_dmx)
  dmx_config_t config = DMX_CONFIG_DEFAULT;
  dmx_driver_install(dmxPort, &config, NULL, 0);
  dmx_set_pin(dmxPort, TX_RS232, RX_RS232, -1); 
  
  memset(dmxData, 0, 513);
  dmx_write(dmxPort, dmxData, 513);

  // 3. CONNEXION WI-FI
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  int tentatives = 0;
  while (WiFi.status() != WL_CONNECTED && tentatives < 10) { // Réduit à 5 sec d'attente
    delay(500);
    tentatives++;
  }

  // 4. RÉSULTAT DE LA CONNEXION
  afficheurOLED.clear();
  if (WiFi.status() == WL_CONNECTED) {
    afficheurOLED.setFont(ArialMT_Plain_24);
    afficheurOLED.drawString(45, 0, "OK");
    afficheurOLED.setFont(ArialMT_Plain_10);
    afficheurOLED.drawString(10, 30, "IP: " + WiFi.localIP().toString());
    udp.begin(artNetPort);
  } else {
    afficheurOLED.setFont(ArialMT_Plain_24);
    afficheurOLED.drawString(30, 0, "NO WIFI");
    if (MODE_TEST) {
      afficheurOLED.setFont(ArialMT_Plain_10);
      afficheurOLED.drawString(10, 30, "=> MODE TEST ACTIF");
    }
  }
  afficheurOLED.display();
}

void loop() {
  // En temps normal, si pas de Wi-Fi on s'arrête. En MODE_TEST, on continue !
  if (!MODE_TEST && WiFi.status() != WL_CONNECTED) return;

  int packetSize = 0;

  // ==========================================================
  // 1A. MODE TEST : GÉNÉRATION D'UNE FAUSSE TRAME ART-NET
  // ==========================================================
  if (MODE_TEST) {
    static unsigned long dernierTest = 0;
    // On simule la réception d'une trame toutes les 40 millisecondes (25 images/sec)
    if (millis() - dernierTest > 40) {
      // --- Création de l'en-tête officiel Art-Net ---
      memcpy(packetBuffer, "Art-Net\0", 8);
      packetBuffer[8] = 0x00;  // OpCode (0x5000 = DMX)
      packetBuffer[9] = 0x50;
      packetBuffer[10] = 0x00; // Version
      packetBuffer[11] = 14;
      packetBuffer[12] = 0;    // Séquence 
      packetBuffer[13] = 0;    // Port Physique
      packetBuffer[14] = 0;    // Univers
      packetBuffer[15] = 0;
      packetBuffer[16] = 0x02; // Longueur des données : 512 (Big Endian)
      packetBuffer[17] = 0x00;

      // --- Création de fausses valeurs DMX ---
      uint8_t effetFondu = (millis() / 10) % 255; // Valeur qui monte de 0 à 255 en boucle
      memset(packetBuffer + 18, 0, 512);          // On éteint les 512 canaux
      packetBuffer[18] = 255;              // Canal 1 : Monte progressivement
      packetBuffer[19] = 0;        // Canal 2 : Descend progressivement
      packetBuffer[20] = 128;                     // Canal 3 : Allumé à fond
      packetBuffer[21] = 180;

      packetSize = 530; // On fait croire au code qu'il a reçu 530 octets
      dernierTest = millis();
    }
  } 
  // ==========================================================
  // 1B. MODE NORMAL : ÉCOUTE DU VRAI RÉSEAU UDP
  // ==========================================================
  else {
    packetSize = udp.parsePacket();
    if (packetSize > 0 && packetSize <= 530) {
      udp.read(packetBuffer, 530);
    }
  }

  // ==========================================================
  // 2. DÉCODAGE DE LA TRAME (Réelle ou Simulée)
  // ==========================================================
  if (packetSize > 0) {
    char artnetHeader[] = "Art-Net\0";
    if (memcmp(packetBuffer, artnetHeader, 8) == 0) {
      
      uint16_t opcode = packetBuffer[8] | (packetBuffer[9] << 8);
      
      if (opcode == 0x5000) { // Si c'est bien de la lumière
        uint16_t dmxLength = (packetBuffer[16] << 8) | packetBuffer[17];
        
        if (dmxLength > 0 && dmxLength <= 512) {
          // On copie les valeurs dans la mémoire DMX physique
          memcpy(dmxData + 1, packetBuffer + 18, dmxLength);
          dmx_write(dmxPort, dmxData, 513);
          paquetsRecus++;
        }
      }
    }
  }

  // ==========================================================
  // 3. ENVOI PHYSIQUE DU SIGNAL DMX 
  // ==========================================================
  if (millis() - dernierEnvoiDMX > 25) {
    dmx_send(dmxPort);
    dernierEnvoiDMX = millis();
  }

  // ==========================================================
  // 4. RAFRAÎCHISSEMENT DE L'ÉCRAN OLED
  // ==========================================================
  if (millis() - dernierRafraichissementOLED > 1000) {
    afficheurOLED.setColor(BLACK);
    afficheurOLED.fillRect(0, 45, 128, 19); 
    afficheurOLED.setColor(WHITE);
    
    afficheurOLED.setFont(ArialMT_Plain_10);
    afficheurOLED.drawString(10, 45, "Art-Net Rx: " + String(paquetsRecus));
    afficheurOLED.display();
    
    dernierRafraichissementOLED = millis();
  }
}


