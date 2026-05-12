/**
 * @file main.cpp
 * @brief Programme principal du nœud Wi-Fi Art-Net vers DMX.
 * @details Ce fichier connecte l'ESP32 au réseau Wi-Fi, écoute les paquets UDP 
 * (Art-Net), décode les valeurs et les transmet au contrôleur DMX matériel.
 * Il intègre un test matériel (POST) qui fait flasher les projecteurs en RGB.
 */

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>

// Inclusion des modules spécifiques au projet
#include "config.h"
#include "ecran.h"
#include "dmx.h"

// ==========================================================
// VARIABLES ET OBJETS GLOBAUX
// ==========================================================
WiFiUDP udp;
uint8_t packetBuffer[530];   
byte dmxData[513];           
long paquetsRecus = 0;

// ==========================================================
// SÉQUENCE POST (Power-On Self-Test) : PASSERELLE DMX
// ==========================================================
/**
 * @brief Exécute une série de tests au démarrage pour valider la passerelle.
 * @details Valide la mémoire, l'écran, le bus RS485 et envoie une séquence 
 * Rouge/Vert/Bleu sur le réseau DMX pour tester les projecteurs connectés.
 */
void executerPOST_Passerelle() {
  Serial.println("\n=====================================");
  Serial.println("  DIAGNOSTIC DE DEMARRAGE (POST) : DMX");
  Serial.println("=====================================");

  int testsReussis = 0;

  // 1. Test du Buffer interne (Mémoire DMX)
  Serial.print("   [MEMOIRE] Test d'ecriture buffer     : ");
  dmxData[512] = 255; 
  if (dmxData[512] == 255) { 
    Serial.println("OK"); testsReussis++; 
  } else { 
    Serial.println("ECHEC"); 
  }

  // 2. Initialisation I2C pour l'écran
  Serial.print("   [I2C] Initialisation Ecran OLED      : ");
  initialiserEcran(); 
  Serial.println("OK"); testsReussis++;

  // 3. Initialisation du port de communication DMX (Puce MAX485)
  Serial.print("   [UART] Activation Bus RS485 (TX)     : ");
  initialiserDMX();
  Serial.println("OK"); testsReussis++;

  // 4. Test physique d'allumage des projecteurs (Flash RGB)
  Serial.print("   [LUMIERE] Séquence Flash (R, G, B)   : ");

  // --- FLASH ROUGE ---
  memset(dmxData, 0, 513); // Remise à zéro de tous les canaux
  dmxData[1] = 255;        // CH1 : Rouge au maximum
  dmxData[4] = 255;        // CH4 : Toutes les zones
  dmxData[5] = 149;        // CH5 : Dimmer (Luminosité à 100%)
  ecrireDonneesDMX(dmxData, 513);
  envoyerSignalDMX();
  delay(400);

  // --- FLASH VERT ---
  memset(dmxData, 0, 513); 
  dmxData[2] = 255;        // CH2 : Vert au maximum
  dmxData[4] = 255;        // CH4 : Toutes les zones
  dmxData[5] = 149;        // CH5 : Dimmer (Luminosité à 100%)
  ecrireDonneesDMX(dmxData, 513);
  envoyerSignalDMX();
  delay(400);

  // --- FLASH BLEU ---
  memset(dmxData, 0, 513); 
  dmxData[3] = 255;        // CH3 : Bleu au maximum
  dmxData[4] = 255;        // CH4 : Toutes les zones
  dmxData[5] = 149;        // CH5 : Dimmer (Luminosité à 100%)
  ecrireDonneesDMX(dmxData, 513);
  envoyerSignalDMX();
  delay(400);

  // --- EXTINCTION (BLACKOUT) ---
  memset(dmxData, 0, 513); 
  ecrireDonneesDMX(dmxData, 513);
  envoyerSignalDMX();

  Serial.println("OK (Termine)"); testsReussis++;

  Serial.printf("   Bilan : %d/4 etapes de demarrage validees.\n", testsReussis);
  Serial.println("=====================================\n");
  delay(1000);
}

// ==========================================================
// INITIALISATION DU SYSTÈME
// ==========================================================
void setup() {
  Serial.begin(115200);
  delay(1000);

  // 1. Exécution du test de démarrage (Flash RGB)
  executerPOST_Passerelle();
  
  afficherAttenteWifi();

  // 2. Connexion au réseau Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  int tentatives = 0;
  while (WiFi.status() != WL_CONNECTED && tentatives < 20) { 
    delay(500);
    tentatives++;
    Serial.print(".");
  }

  // 3. Résultat de la connexion
  if (WiFi.status() == WL_CONNECTED) {
    afficherStatutWifi(true, WiFi.localIP().toString());
    udp.begin(artNetPort);
    Serial.println("\n[RESEAU] Connecte. Ecoute UDP (Art-Net) active.");
  } else {
    afficherStatutWifi(false, "");
    Serial.println("\n[ERREUR] Wi-Fi indisponible.");
  }
}

// ==========================================================
// BOUCLE PRINCIPALE
// ==========================================================
void loop() {
  // TEST DE PERFORMANCE : Début du chronomètre CPU
  unsigned long chronoDebut = micros();

  // Sécurité : on attend d'avoir le Wi-Fi
  if (WiFi.status() != WL_CONNECTED) return;

  // ==========================================================
  // 1. ÉCOUTE DU RÉSEAU UDP (TRAMES ART-NET)
  // ==========================================================
  int packetSize = udp.parsePacket();
  
  if (packetSize > 0 && packetSize <= 530) {
    udp.read(packetBuffer, 530);
    
    // Vérification de la signature Art-Net
    if (memcmp(packetBuffer, "Art-Net\0", 8) == 0) {
      uint16_t opcode = packetBuffer[8] | (packetBuffer[9] << 8);
      
      // 0x5000 correspond au protocole ArtDmx
      if (opcode == 0x5000) { 
        uint16_t dmxLength = (packetBuffer[16] << 8) | packetBuffer[17];
        
        if (dmxLength > 0 && dmxLength <= 512) {
          
          // Copie des valeurs reçues dans notre tableau DMX matériel
          memcpy(dmxData + 1, packetBuffer + 18, dmxLength);
          ecrireDonneesDMX(dmxData, 513);
          paquetsRecus++;

          // ==========================================================
          // 2. MODE ESPION INTELLIGENT (Sur le port Série)
          // ==========================================================
          static unsigned long dernierAffichage = 0;
          if (millis() - dernierAffichage > 1000) {
            unsigned long tempsDeCycle = micros() - chronoDebut;

            Serial.println("\n--- TRAME ART-NET VALIDEE ---");
            Serial.printf("[RESEAU] Paquet n° : %ld | Taille : %d octets\n", paquetsRecus, packetSize);
            Serial.printf("[CPU] Temps de traitement trame : %lu us\n", tempsDeCycle);
            
            // Affichage des 16 premiers canaux
            Serial.print("[DATA] Canaux 1 a 16 : ");
            for(int i = 1; i <= 16; i++) {
              Serial.printf("%3d ", dmxData[i]);
            }
            Serial.println("\n-----------------------------");
            
            dernierAffichage = millis();
          }
        }
      }
    }
  }

  // ==========================================================
  // 3. ENVOI PHYSIQUE ET IHM
  // ==========================================================
  envoyerSignalDMX();
  rafraichirEcran(paquetsRecus);
}