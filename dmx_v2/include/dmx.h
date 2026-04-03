#ifndef DMX_H
#define DMX_H

#include <Arduino.h>

void initialiserDMX();
void ecrireDonneesDMX(uint8_t* donnees, uint16_t taille);
void envoyerSignalDMX();

#endif