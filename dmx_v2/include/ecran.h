#ifndef ECRAN_H
#define ECRAN_H

#include <Arduino.h>

void initialiserEcran();
void afficherAttenteWifi();
void afficherStatutWifi(bool connecte, String ip);
void rafraichirEcran(long paquetsRecus);

#endif