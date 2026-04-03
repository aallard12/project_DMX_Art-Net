#include "ecran.h"
#include <SSD1306.h>
#include "config.h"

SSD1306Wire afficheurOLED(ADD_OLED, SDA, SCL);
unsigned long dernierRafraichissementOLED = 0;

void initialiserEcran() {
  afficheurOLED.init();
  afficheurOLED.flipScreenVertically();
  afficheurOLED.clear();
}

void afficherAttenteWifi() {
  afficheurOLED.clear();
  afficheurOLED.setFont(ArialMT_Plain_16);
  afficheurOLED.drawString(10, 20, "Connexion Wi-Fi...");
  afficheurOLED.display();
}

void afficherStatutWifi(bool connecte, String ip) {
  afficheurOLED.clear();
  if (connecte) {
    afficheurOLED.setFont(ArialMT_Plain_24);
    afficheurOLED.drawString(45, 0, "OK");
    afficheurOLED.setFont(ArialMT_Plain_10);
    afficheurOLED.drawString(10, 30, "IP: " + ip);
  } else {
    afficheurOLED.setFont(ArialMT_Plain_24);
    afficheurOLED.drawString(30, 0, "NO WIFI");
  }
  afficheurOLED.display();
}

void rafraichirEcran(long paquetsRecus) {
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