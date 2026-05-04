/**
 * @file main.cpp
 * @brief Programme principal du nœud Wi-Fi Art-Net vers DMX.
 * @details Ce fichier orchestre l'ensemble du système : il connecte l'ESP32 
 * au réseau Wi-Fi, écoute les paquets UDP entrants (protocole Art-Net), 
 * décode les valeurs de lumière et les transmet au contrôleur DMX matériel 
 * tout en mettant à jour l'écran de contrôle OLED.
 */

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>

// Inclusion des modules spécifiques au projet
#include "config.h"
#include "ecran.h"
#include "dmx.h"

/**
 * @brief Objet gérant la communication UDP via le Wi-Fi.
 */
WiFiUDP udp;

/**
 * @brief Mémoire tampon (buffer) pour stocker le paquet réseau brut reçu.
 * @details La taille est fixée à 530 octets, ce qui est suffisant pour 
 * contenir l'en-tête Art-Net (18 octets) et les 512 canaux DMX.
 */
uint8_t packetBuffer[530];   

/**
 * @brief Tableau contenant l'état actuel des 512 canaux de lumière.
 * @details La case dmxData[0] est réservée au "Start Code" du protocole DMX 
 * (qui vaut généralement 0). Les canaux d'éclairage vont de l'index 1 à 512.
 */
byte dmxData[513];           

/**
 * @brief Compteur global du nombre de trames Art-Net valides reçues.
 * @details Utilisé pour l'affichage sur l'écran OLED afin de vérifier 
 * que la communication réseau est active.
 */
long paquetsRecus = 0;

/**
 * @brief Fonction d'initialisation du microcontrôleur.
 * @details Exécutée une seule fois au démarrage. Elle initialise le port série 
 * (pour le débogage), l'écran OLED, le contrôleur DMX RS485, puis tente de 
 * se connecter au réseau Wi-Fi avant d'ouvrir le port UDP d'écoute.
 */
void setup() {
  Serial.begin(115200);

  // Initialisation des périphériques matériels
  initialiserEcran();
  afficherAttenteWifi();
  initialiserDMX();

  // 1. CONNEXION WI-FI
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  int tentatives = 0;
  // On attend un maximum de 20 cycles (10 secondes) pour la connexion
  while (WiFi.status() != WL_CONNECTED && tentatives < 20) { 
    delay(500);
    tentatives++;
  }

  // 2. RÉSULTAT DE LA CONNEXION
  if (WiFi.status() == WL_CONNECTED) {
    afficherStatutWifi(true, WiFi.localIP().toString());
    
    // Si le Wi-Fi est opérationnel, on ouvre le port UDP (6454 par défaut)
    udp.begin(artNetPort);
  } else {
    // Si échec, on affiche l'erreur sur l'écran OLED
    afficherStatutWifi(false, "");
  }
}

/**
 * @brief Boucle principale d'exécution.
 * @details Tourne en continu. Ses rôles principaux sont :
 * 1. Vérifier si un nouveau paquet UDP est arrivé.
 * 2. Vérifier s'il s'agit bien d'une trame Art-Net DMX valide.
 * 3. Extraire les données et les envoyer au buffer DMX.
 * 4. Déclencher l'envoi physique du signal DMX et rafraîchir l'écran.
 */
void loop() {
  // Sécurité : s'il n'y a pas de Wi-Fi, on ne peut rien faire, on recommence la boucle.
  if (WiFi.status() != WL_CONNECTED) return;

  // ==========================================================
  // 1. ÉCOUTE DU RÉSEAU UDP (LES TRAMES DU SERVEUR)
  // ==========================================================
  int packetSize = udp.parsePacket();
  
  // Si on a reçu quelque chose dont la taille correspond à du DMX
  if (packetSize > 0 && packetSize <= 530) {
    // On copie le contenu du réseau dans notre buffer local
    udp.read(packetBuffer, 530);
    
    // ==========================================================
    // 2. DÉCODAGE DE LA TRAME ART-NET
    // ==========================================================
    // On vérifie que le paquet commence bien par la signature "Art-Net\0"
    char artnetHeader[] = "Art-Net\0";
    if (memcmp(packetBuffer, artnetHeader, 8) == 0) {
      
      // Extraction de l'OpCode (code d'opération) sur 2 octets
      uint16_t opcode = packetBuffer[8] | (packetBuffer[9] << 8);
      
      // 0x5000 correspond à un paquet de données DMX standard (ArtDmx)
      if (opcode == 0x5000) { 
        // Extraction de la longueur des données DMX (généralement 512)
        uint16_t dmxLength = (packetBuffer[16] << 8) | packetBuffer[17];
        
        if (dmxLength > 0 && dmxLength <= 512) {
          // On copie les valeurs réseau directement dans le tableau DMX physique
          // (On ignore les 18 premiers octets d'en-tête réseau)
          memcpy(dmxData + 1, packetBuffer + 18, dmxLength);
          
          // ==========================================================
          // DÉBOGAGE : ESPIONNAGE DES 512 CANAUX SUR LE MONITEUR SÉRIE
          // ==========================================================
          static unsigned long dernierAffichage = 0;
          
          // On affiche le tableau 1 seule fois par seconde pour ne pas saturer le port série
          if (millis() - dernierAffichage > 1000) {
            Serial.println("\n=== NOUVELLE TRAME ART-NET (512 Canaux) ===");
            
            // Formatage en tableau (16 colonnes par ligne)
            for (int i = 1; i <= 512; i += 16) {
              // Écriture de l'en-tête de la ligne (Ex: "CH 001-016 : ")
              Serial.printf("CH %03d-%03d : ", i, i + 15);
              
              // Écriture des 16 valeurs DMX de cette ligne
              for (int j = 0; j < 16; j++) {
                if (i + j <= 512) {
                  // %3d permet d'aligner les nombres sur 3 caractères pour la lisibilité
                  Serial.printf("%3d ", dmxData[i + j]);
                }
              }
              Serial.println(); 
            }
            Serial.println("===========================================");
            
            dernierAffichage = millis();
          }
          
          // On valide les données vers le module DMX et on incrémente le compteur
          ecrireDonneesDMX(dmxData, 513);
          paquetsRecus++;
        }
      }
    }
  }

  // ==========================================================
  // 3. TÂCHES DE FOND (Envoi physique et mise à jour écran)
  // ==========================================================
  // Ces fonctions gèrent leurs propres timers (millis) en interne pour 
  // ne s'exécuter qu'au bon moment (ex: toutes les 25ms pour le DMX).
  envoyerSignalDMX();
  rafraichirEcran(paquetsRecus);
}