#include <Arduino.h>
#include "esp32_snir.h"
#include <SSD1306.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Keypad.h>
#include <Adafruit_NeoPixel.h>
#include <esp_dmx.h>


// --- INITIALISATION ---
SSD1306Wire afficheurOLED(ADD_OLED, SDA, SCL);
OneWire oneWire(TEMP); 
DallasTemperature capteurTemp(&oneWire);
Adafruit_NeoPixel pixels(NB_PIXELS, DATALEDS, NEO_RGB + NEO_KHZ800);

float derniereTemp = 0;
// Variable pour mémoriser la couleur choisie (0 = aucune, 1 = Rouge, 2 = Vert, 3 = Bleu)
int couleurSelectionnee = 0; 

void setup() {
  Serial.begin(115200);
  
  pinMode(SW, INPUT_PULLUP); 
  pinMode(BP1, INPUT);
  pinMode(BP2, INPUT);
  pinMode(BP3, INPUT);

  afficheurOLED.init();
  afficheurOLED.flipScreenVertically();
  
  capteurTemp.begin();
  capteurTemp.setResolution(12);
  capteurTemp.setWaitForConversion(false); 
  capteurTemp.requestTemperatures();
  
  pixels.begin();
  // On met la luminosité globale au maximum pour que le potentiomètre ait toute la plage
  pixels.setBrightness(255); 
  pixels.clear();
  pixels.show();
}

void loop() {
  // 1. LECTURE DES ENTRÉES
  bool positionBasse = (digitalRead(SW) == LOW);
  bool appuiBP1 = (digitalRead(BP1) == LOW); 
  bool appuiBP2 = (digitalRead(BP2) == LOW); 
  bool appuiBP3 = (digitalRead(BP3) == LOW); 
  
  // Lecture du potentiomètre (0 à 4095) convertie en intensité (0 à 255)
  int valeurRV1 = analogRead(ADC_LDR);
  int intensite = map(valeurRV1, 0, 4095, 0, 255);

  // 2. GESTION DU CAPTEUR
  if (capteurTemp.isConversionComplete()) {
    derniereTemp = capteurTemp.getTempCByIndex(0);
    capteurTemp.requestTemperatures(); 
  }

  // 3. AFFICHAGE
  afficheurOLED.clear();

  if (positionBasse) {
    // --- MODE BONJOUR ---
    // J'ai remonté le "Bonjour !" à Y=10 pour faire de la place en dessous
    afficheurOLED.setFont(ArialMT_Plain_24);
    afficheurOLED.drawString(20, 10, "Bonjour !");
    
    // Si on appuie sur un bouton, on mémorise le choix
    if (appuiBP1) couleurSelectionnee = 1; // Rouge
    if (appuiBP2) couleurSelectionnee = 2; // Vert
    if (appuiBP3) couleurSelectionnee = 3; // Bleu

    // NOUVEAU : Affichage de l'intensité sur l'écran si une couleur est choisie
    if (couleurSelectionnee != 0) {
      afficheurOLED.setFont(ArialMT_Plain_16);
      afficheurOLED.drawString(10, 40, "Intensite : " + String(intensite));
    }

    pixels.clear();

    if (couleurSelectionnee == 0) {
      // ÉTAT INITIAL : Les 3 couleurs sont allumées (en 3 blocs) pour montrer les choix
      for(int i = 0; i < 20; i++) pixels.setPixelColor(i, 50, 0, 0);       // Rouge
      for(int i = 20; i < 40; i++) pixels.setPixelColor(i, 0, 50, 0);      // Vert
      for(int i = 40; i < 60; i++) pixels.setPixelColor(i, 0, 0, 50);      // Bleu
    } 
    else if (couleurSelectionnee == 1) {
      // ROUGE sélectionné : On applique l'intensité lue par le potentiomètre
      for(int i = 0; i < NB_PIXELS; i++) pixels.setPixelColor(i, intensite, 0, 0); 
    }
    else if (couleurSelectionnee == 2) {
      // VERT sélectionné
      for(int i = 0; i < NB_PIXELS; i++) pixels.setPixelColor(i, 0, intensite, 0); 
    }
    else if (couleurSelectionnee == 3) {
      // BLEU sélectionné
      for(int i = 0; i < NB_PIXELS; i++) pixels.setPixelColor(i, 0, 0, intensite); 
    }
    
    pixels.show();
  } 
  else {
    // --- MODE TEMPÉRATURE ---
    // On remet le choix de la couleur à 0 pour la prochaine fois qu'on repasse en "Bonjour"
    couleurSelectionnee = 0; 
    
    int nbLedsAllumer = 0;
    if (derniereTemp > 20.0) {
      nbLedsAllumer = (int)((derniereTemp - 20.0) * 4.0); 
    }

    afficheurOLED.setFont(ArialMT_Plain_10);
    afficheurOLED.drawString(15, 0, "Temperature");
    afficheurOLED.setFont(ArialMT_Plain_24);
    afficheurOLED.drawString(30, 20, String(derniereTemp, 2) + " C");

    pixels.clear(); 
    for(int i = 0; i < nbLedsAllumer && i < NB_PIXELS; i++) {
      // On bride volontairement l'intensité à 50 max dans ce mode pour ne pas éblouir
      if (i < 20)      pixels.setPixelColor(i, 0, 0, 50); // BLEU
      else if (i < 40) pixels.setPixelColor(i, 0, 50, 0); // VERT
      else             pixels.setPixelColor(i, 50, 0, 0); // ROUGE
    }
    pixels.show();
  }

  afficheurOLED.display();
  delay(20); 
}