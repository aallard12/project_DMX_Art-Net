#include <Arduino.h>

#include "esp32_snir.h" 

// 2. Inclusion de tes classes (qui sont dans le dossier include/)
#include "ProjecteurLED.h"
#include "EcranOLED.h"
#include "RecepteurDMX.h"

// ===================================================================================
// INSTANCIATION DES OBJETS
// ===================================================================================
ProjecteurLED monRuban;
EcranOLED monEcran(ADD_OLED, SDA, SCL);
RecepteurDMX monDMX;

#define DMX_START 1

void setup() {
  Serial.begin(115200);

  monEcran.initialiser();
  monRuban.initialiser();
  monDMX.initialiser(16); // Broche RX = 16

  Serial.println("Systeme OOP pret !");
}

void loop() {
  // 1. Mise à jour de l'état
  monDMX.ecouter();
  monRuban.actualiserHorloge();

  // 2. Lecture des canaux
  uint8_t r         = monDMX.lireCanal(DMX_START);
  uint8_t g         = monDMX.lireCanal(DMX_START + 1);
  uint8_t b         = monDMX.lireCanal(DMX_START + 2);
  uint8_t ch_zone   = monDMX.lireCanal(DMX_START + 3);
  uint8_t ch_mode   = monDMX.lireCanal(DMX_START + 4);
  uint8_t ch_effets = monDMX.lireCanal(DMX_START + 5);

  // 3. Logique de sélection de la zone (Canal 4)
  int idxDebut = 0, idxFin = 0, indexSauvegarde = -1;
  String nomZone = "";

  switch (ch_zone) {
    case 0 ... 49:     idxDebut = 0;  idxFin = 15; nomZone = "Z1 (1-15)";  indexSauvegarde = 0; break;
    case 50 ... 99:    idxDebut = 15; idxFin = 30; nomZone = "Z2 (16-30)"; indexSauvegarde = 1; break;
    case 100 ... 149:  idxDebut = 30; idxFin = 45; nomZone = "Z3 (31-45)"; indexSauvegarde = 2; break;
    case 150 ... 199:  idxDebut = 45; idxFin = 60; nomZone = "Z4 (46-60)"; indexSauvegarde = 3; break;
    case 200 ... 249:  idxDebut = 0;  idxFin = 60; nomZone = "Z1+2+3+4 Memoire"; break;
    case 250 ... 255:  idxDebut = 0;  idxFin = 60; nomZone = "Z1+2+3+4 Unifiees"; break;
  }

  if (idxFin > NB_PIXELS) idxFin = NB_PIXELS;

  // Sauvegarde en mémoire si on est sur une zone isolée
  if (indexSauvegarde != -1) {
    monRuban.sauvegarderCouleurZone(indexSauvegarde, r, g, b);
  }

  // 4. Base d'allumage
  monRuban.toutEteindre();

  switch (ch_zone) {
    case 0 ... 199:
    case 250 ... 255:
      monRuban.peindreZoneFixe(idxDebut, idxFin, r, g, b);
      break;
    case 200 ... 249:
      monRuban.peindreToutesZonesMemorisees();
      break;
  }

  // 5. Application des effets (Canaux 5 et 6)
  String nomMode = "DIMMER";
  String infoEffet = "";

  switch (ch_mode) {
    case 250 ... 255:
      nomMode = "EFFETS (CH6)";
      infoEffet = String(ch_effets);
      
      switch (ch_effets) {
        case 0 ... 25:    monRuban.effet01_Rainbow(idxDebut, idxFin); break;
        case 26 ... 51:   monRuban.effet02_Chenillard(idxDebut, idxFin); break;
        case 52 ... 76:   monRuban.effet03_Confetti(idxDebut, idxFin); break;
        case 77 ... 102:  monRuban.effet04_Sinelon(idxDebut, idxFin); break;
        case 103 ... 127: monRuban.effet05_BPM(idxDebut, idxFin); break;
        case 128 ... 153: monRuban.effet06_Juggle(idxDebut, idxFin); break;
        case 154 ... 179: monRuban.effet07_Police(idxDebut, idxFin); break;
        case 180 ... 204: monRuban.effet08_WarpDrive(idxDebut, idxFin); break;
        case 205 ... 230: monRuban.effet09_Breathing(idxDebut, idxFin); break;
        case 231 ... 255: monRuban.effet10_Feu(idxDebut, idxFin); break;
      }
      break;

    case 150 ... 249:
      nomMode = "STROBE";
      monRuban.appliquerStrobe(idxDebut, idxFin, ch_mode);
      break;

    case 0 ... 149:
      nomMode = "DIMMER";
      uint8_t luminosite = map(ch_mode, 0, 149, 0, 255);
      monRuban.appliquerDimmerGlobal(luminosite);
      break;
  }

  // 6. Rendu final
  monRuban.afficher();
  monEcran.actualiser(nomZone, nomMode, infoEffet, monDMX.getTramesRecues());
}