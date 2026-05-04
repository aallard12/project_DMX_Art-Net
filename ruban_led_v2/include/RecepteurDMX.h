/**
 * @file RecepteurDMX.h
 * @brief En-tête de la classe RecepteurDMX.
 * @details Ce fichier définit une classe orientée objet permettant de 
 * gérer la réception de signaux DMX512 via l'interface matérielle UART 
 * de l'ESP32 et un module RS485 (MAX485).
 */

#pragma once

#include <Arduino.h>  
#include <esp_dmx.h> 

/**
 * @class RecepteurDMX
 * @brief Classe gérant l'écoute et le décodage du signal DMX matériel.
 * @details Encapsule l'utilisation de la bibliothèque `esp_dmx` pour 
 * configurer le port série, écouter les trames entrantes et stocker 
 * les valeurs des 512 canaux dans un tableau mémoire accessible.
 */
class RecepteurDMX {
  private:
    /** * @brief Identifiant du port UART matériel utilisé. 
     * @details Généralement 1 ou 2 sur l'ESP32.
     */
    dmx_port_t dmxPort;
    
    /** * @brief Tableau mémoire stockant la dernière trame DMX reçue.
     * @details Contient 513 octets : l'index 0 est le Start Code, 
     * les index 1 à 512 contiennent les valeurs des canaux DMX.
     */
    byte dmxData[DMX_PACKET_SIZE];
    
    /** * @brief Compteur du nombre total de trames valides reçues. 
     */
    long tramesRecues;

  public:
    /**
     * @brief Constructeur par défaut de la classe.
     * @details Initialise le port DMX (par défaut port 1) et remet 
     * les compteurs et le tableau de données à zéro.
     */
    RecepteurDMX();

    /**
     * @brief Configure et démarre le driver DMX en mode réception.
     * @param pinRx Le numéro de la broche de l'ESP32 connectée à la sortie RO du MAX485.
     * @details Doit être appelée une seule fois dans le `setup()`.
     */
    void initialiser(int pinRx);

    /**
     * @brief Vérifie si une nouvelle trame DMX est arrivée sur le réseau.
     * @return `true` si une trame valide et sans erreur a été reçue et copiée en mémoire, `false` sinon.
     * @details Cette méthode doit être appelée le plus souvent possible 
     * dans la boucle principale (`loop()`) pour ne rater aucune donnée.
     */
    bool ecouter();

    /**
     * @brief Récupère la valeur actuelle d'un canal DMX spécifique.
     * @param canal Le numéro du canal DMX souhaité (de 1 à 512).
     * @return La valeur de la lumière pour ce canal (de 0 à 255).
     * @details Sécurisée en interne : si le numéro de canal demandé est 
     * en dehors des limites (ex: 600), la méthode doit retourner 0.
     */
    uint8_t lireCanal(int canal);

    /**
     * @brief Récupère le compteur de trames.
     * @return Le nombre de paquets DMX décodés avec succès depuis le démarrage.
     * @details Utile pour l'affichage sur un écran de contrôle pour vérifier 
     * que la liaison filaire fonctionne.
     */
    long getTramesRecues();
};