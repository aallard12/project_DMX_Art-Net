#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>

// Inclusion de nos modules
#include "config.h"
#include "ecran.h"
#include "dmx.h"

WiFiUDP udp;
uint8_t packetBuffer[530];   
byte dmxData[513];           
long paquetsRecus = 0;

void setup() {
  Serial.begin(115200);

  // Initialisation du matériel 
  initialiserEcran();
  afficherAttenteWifi();
  initialiserDMX();

  // 1. CONNEXION WI-FI
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  int tentatives = 0;
  while (WiFi.status() != WL_CONNECTED && tentatives < 20) { 
    delay(500);
    tentatives++;
  }

  // 2. RÉSULTAT DE LA CONNEXION
  if (WiFi.status() == WL_CONNECTED) {
    afficherStatutWifi(true, WiFi.localIP().toString());
    
    // Si on a le Wi-Fi, on ouvre le port pour écouter le serveur !
    udp.begin(artNetPort);
  } else {
    afficherStatutWifi(false, "");
  }
}

void loop() {
  // S'il n'y a pas de Wi-Fi, on attend
  if (WiFi.status() != WL_CONNECTED) return;

  // ==========================================================
  // 1. ÉCOUTE DU RÉSEAU UDP (LES TRAMES DU SERVEUR)
  // ==========================================================
  int packetSize = udp.parsePacket();
  if (packetSize > 0 && packetSize <= 530) {
    udp.read(packetBuffer, 530);
    
    // ==========================================================
    // 2. DÉCODAGE DE LA TRAME ART-NET
    // ==========================================================
    char artnetHeader[] = "Art-Net\0";
    if (memcmp(packetBuffer, artnetHeader, 8) == 0) {
      
      uint16_t opcode = packetBuffer[8] | (packetBuffer[9] << 8);
      
      if (opcode == 0x5000) { // Si le serveur envoie bien de la lumière
        uint16_t dmxLength = (packetBuffer[16] << 8) | packetBuffer[17];
        
        if (dmxLength > 0 && dmxLength <= 512) {
          // On copie les valeurs dans le tableau (à partir de la case 1)
          memcpy(dmxData + 1, packetBuffer + 18, dmxLength);
          
          // --- ESPIONNAGE DES 512 CANAUX (Avec numéros) ---
          static unsigned long dernierAffichage = 0;
          
          // On affiche le tableau 1 seule fois par seconde pour ne pas faire planter l'ordi
          if (millis() - dernierAffichage > 1000) {
            Serial.println("\n=== NOUVELLE TRAME ART-NET (512 Canaux) ===");
            
            // On avance de 16 en 16 pour créer des lignes
            for (int i = 1; i <= 512; i += 16) {
              
              // 1. On écrit l'en-tête de la ligne (Ex: "CH 001-016 : ")
              Serial.printf("CH %03d-%03d : ", i, i + 15);
              
              // 2. On écrit les 16 valeurs DMX de cette ligne
              for (int j = 0; j < 16; j++) {
                if (i + j <= 512) {
                  // %3d permet d'aligner les nombres sur 3 caractères (ex: "  0", " 45", "255")
                  Serial.printf("%3d ", dmxData[i + j]);
                }
              }
              Serial.println(); // On passe à la ligne suivante
            }
            Serial.println("===========================================");
            
            dernierAffichage = millis();
          }
          // ------------------------------------------------

          // On envoie les données à notre module DMX matériel
          ecrireDonneesDMX(dmxData, 513);
          paquetsRecus++;
        }
      }
    }
  }

  // ==========================================================
  // 3. TÂCHES DE FOND (Envoi physique et mise à jour écran)
  // ==========================================================
  envoyerSignalDMX();
  rafraichirEcran(paquetsRecus);
}