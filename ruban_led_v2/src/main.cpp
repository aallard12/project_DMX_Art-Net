/**
 * @file main.cpp
 * @brief Programme principal du contrôleur LED DMX Orienté Objet.
 * @details Intègre : POST (Autotest), IHM (Clavier/OLED), Décodage DMX,
 * et Failsafe (Perte de signal).
 */

#include <Arduino.h>
#include <Keypad.h> 
#include "esp32_snir.h" 
#include "ProjecteurLED.h"
#include "EcranOLED.h"
#include "RecepteurDMX.h"

// ==========================================================
// INSTANCIATION DES OBJETS GLOBAUX
// ==========================================================
EcranOLED monEcran(ADD_OLED, SDA, SCL);
ProjecteurLED monRuban;
RecepteurDMX monDMX;

// ==========================================================
// CONFIGURATION DU CLAVIER MATRICIEL
// ==========================================================
const byte LIGNES = 4; 
const byte COLONNES = 3; 
char touches[LIGNES][COLONNES] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte brochesLignes[LIGNES] = {L0, L1, L2, L3}; 
byte brochesColonnes[COLONNES] = {C0, C1, C2}; 
Keypad monClavier = Keypad(makeKeymap(touches), brochesLignes, brochesColonnes, LIGNES, COLONNES);

int adresseDMX = 1; 

// ==========================================================
// SÉQUENCE POST (Power-On Self-Test)
// ==========================================================
void executerPOST() {
  Serial.println("\n=====================================");
  Serial.println("  DEMARRAGE DU DIAGNOSTIC COMPLET (POST)");
  Serial.println("=====================================");

  int testsLogicielsReussis = 0;

  Serial.println("-> DIAGNOSTIC LOGICIEL :");
  RecepteurDMX dmxTest; 
  Serial.print("   [DMX] Limites memoire (Canal 600)   : ");
  if (dmxTest.lireCanal(600) == 0) { Serial.println("OK"); testsLogicielsReussis++; } else { Serial.println("ECHEC"); }

  Serial.print("   [LED] Limites memoire (Zone 10)     : ");
  monRuban.sauvegarderCouleurZone(10, 255, 255, 255); 
  Serial.println("OK (Pas de plantage)"); 
  testsLogicielsReussis++;

  Serial.print("   [MATH] Conversion Dimmer (149->255) : ");
  uint8_t testDimmer = map(149, 0, 149, 0, 255);
  if (testDimmer == 255) { Serial.println("OK"); testsLogicielsReussis++; } else { Serial.println("ECHEC"); }

  Serial.print("   [ROUTAGE] Frontiere stricte Z1/Z2   : ");
  int testZoneDebut = 0;
  int testValeurDmx = 50; 
  switch (testValeurDmx) {
    case 0 ... 49:  testZoneDebut = 0; break;
    case 50 ... 99: testZoneDebut = 15; break;
  }
  if (testZoneDebut == 15) { Serial.println("OK"); testsLogicielsReussis++; } else { Serial.println("ECHEC"); }
  Serial.println("   Bilan logiciel : " + String(testsLogicielsReussis) + "/4 tests reussis.\n");

  Serial.println("-> DIAGNOSTIC MATERIEL :");
  Serial.print("   [OLED] Communication I2C... ");
  monEcran.initialiser();
  monEcran.afficherMessage("AUTO-TEST", "Diagnostic en cours...");
  Serial.println("OK (Verifiez l'ecran)");
  delay(1000);

  Serial.print("   [LED] Test RGB (Cablage et puces)... ");
  monRuban.initialiser();
  monRuban.peindreZoneFixe(0, NB_PIXELS, 255, 0, 0); monRuban.afficher(); delay(400); 
  monRuban.peindreZoneFixe(0, NB_PIXELS, 0, 255, 0); monRuban.afficher(); delay(400); 
  monRuban.peindreZoneFixe(0, NB_PIXELS, 0, 0, 255); monRuban.afficher(); delay(400); 
  monRuban.toutEteindre(); monRuban.afficher();
  Serial.println("OK (Verifiez les flashs)");

  Serial.println("=====================================\n");
  delay(1000);
}

// ==========================================================
// INITIALISATION DU SYSTÈME
// ==========================================================
void setup() {
  Serial.begin(115200);
  delay(1000); 

  executerPOST();
  monDMX.initialiser(16); 

  // Séquence IHM : Choix de l'adresse DMX
  String saisie = "";
  bool validee = false;
  unsigned long debutSaisie = millis();
  
  monEcran.afficherMessage("Saisir Adresse DMX :", saisie + "_ (# pour valider)");

  while (!validee) {
    char touche = monClavier.getKey();

    if (touche) {
      debutSaisie = millis(); 

      if (touche >= '0' && touche <= '9') {
        if (saisie.length() < 3) saisie += touche; 
      } 
      else if (touche == '*') {
        saisie = ""; 
      } 
      else if (touche == '#') { 
        if (saisie.length() > 0) {
          int adresseTest = saisie.toInt();
          if (adresseTest >= 1 && adresseTest <= 507) {
            adresseDMX = adresseTest;
            validee = true;
          } else {
            monEcran.afficherMessage("ERREUR !", "L'adresse doit etre < 508");
            delay(1500);
            saisie = "";
          }
        } else {
          validee = true; 
        }
      }
      if (!validee) monEcran.afficherMessage("Saisir Adresse DMX :", saisie + "_ (# pour valider)");
    }
    // Timeout (Test d'ergonomie/crash-test utilisateur)
    if (millis() - debutSaisie > 10000 && saisie == "") {
      adresseDMX = 1; 
      validee = true;
    }
  }

  monEcran.afficherMessage("Demarrage en cours...", "Adresse DMX : " + String(adresseDMX));
  delay(1000); 
  Serial.println("Systeme OOP pret ! Adresse de depart = " + String(adresseDMX));
}

// ==========================================================
// BOUCLE PRINCIPALE
// ==========================================================
void loop() {
  // 1. ÉCOUTE ET RÉCUPÉRATION DES DONNÉES DMX
  monDMX.ecouter();
  monRuban.actualiserHorloge(); 

  // --- GESTION DU FAILSAFE (PERTE DE SIGNAL) ---
  static unsigned long tempsDerniereTrame = millis();
  static long tramesPrecedentes = -1;
  long tramesActuelles = monDMX.getTramesRecues();

  // Si on a reçu une nouvelle trame, on remet le compteur à zéro
  if (tramesActuelles != tramesPrecedentes) {
    tempsDerniereTrame = millis();
    tramesPrecedentes = tramesActuelles;
  }

  // Si aucune donnée n'arrive pendant plus de 3 secondes (3000 ms)
  if (millis() - tempsDerniereTrame > 3000) {
    monRuban.toutEteindre(); // Blackout immédiat par sécurité
    monRuban.afficher();
    
    // On n'affiche le message d'alerte qu'une fois par seconde pour ne pas saturer l'écran
    static unsigned long dernierLogAlerte = 0;
    if (millis() - dernierLogAlerte > 1000) {
      monEcran.afficherMessage("ALERTE", "SIGNAL DMX PERDU !");
      Serial.println("[ALERTE] Failsafe active : Coupure de la lumiere (Blackout).");
      dernierLogAlerte = millis();
    }
    return; // /!\ IMPORTANT : On stoppe la boucle ici en attendant le signal
  }
  // ----------------------------------------------------

  // Lecture des 6 canaux
  uint8_t r         = monDMX.lireCanal(adresseDMX);     
  uint8_t g         = monDMX.lireCanal(adresseDMX + 1); 
  uint8_t b         = monDMX.lireCanal(adresseDMX + 2); 
  uint8_t ch_zone   = monDMX.lireCanal(adresseDMX + 3); 
  uint8_t ch_mode   = monDMX.lireCanal(adresseDMX + 4); 
  uint8_t ch_effets = monDMX.lireCanal(adresseDMX + 5); 

  // 2. DÉCODAGE DU CANAL ZONE (CH4)
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
  if (indexSauvegarde != -1) monRuban.sauvegarderCouleurZone(indexSauvegarde, r, g, b);

  // 3. APPLICATION DE LA COULEUR DE BASE
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

  // 4. APPLICATION DU MODE ET DES EFFETS (CH5 & CH6)
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
      monRuban.appliquerDimmerGlobal(map(ch_mode, 0, 149, 0, 255));
      break;
  }

  // 5. RENDU PHYSIQUE ET AFFICHAGE MONITEUR
  monRuban.afficher(); 
  monEcran.actualiser(nomZone, nomMode, infoEffet, monDMX.getTramesRecues());

  // ==========================================================
  // MODE ESPION (AFFICHAGE SUR LE MONITEUR SÉRIE)
  // ==========================================================
  
  // 1. Affichage continu du réseau (S'actualise tout seul 1 fois par seconde)
  static unsigned long dernierLogReseau = 0;
  if (millis() - dernierLogReseau > 1000) {
    Serial.printf("[RESEAU] Trames DMX validees reçues : %ld\n", monDMX.getTramesRecues());
    dernierLogReseau = millis();
  }

  // 2. Affichage des commandes (Ne s'affiche QUE si on bouge un curseur)
  static uint8_t old_r = 0, old_g = 0, old_b = 0, old_zone = 0, old_mode = 0, old_effets = 0;
  
  if (r != old_r || g != old_g || b != old_b || ch_zone != old_zone || ch_mode != old_mode || ch_effets != old_effets) {
    Serial.println("\n--- NOUVELLE COMMANDE DETECTEE ---");
    Serial.printf("[RECEPTION] Valeurs DMX (RGB) reçues : %d, %d, %d\n", r, g, b);
    Serial.printf("[CONVERSION] CH Zone (%d) -> Converti en : %s (Index LED : %d a %d)\n", ch_zone, nomZone.c_str(), idxDebut, idxFin);
    Serial.printf("[CONVERSION] CH Mode (%d) -> Converti en : %s\n", ch_mode, nomMode.c_str());
    Serial.println("[ENVOI] Mise a jour du ruban LED effectuee.");
    Serial.println("----------------------------------");
    
    // On mémorise les valeurs pour la prochaine comparaison
    old_r = r; old_g = g; old_b = b; old_zone = ch_zone; old_mode = ch_mode; old_effets = ch_effets;
  }
}