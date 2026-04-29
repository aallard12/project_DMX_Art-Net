/**
 * @file testclassbdd.h
 * @brief Déclaration de la classe de tests TestClassBDD
 * @author Antoine ALLARD
 * @date 10/04/2026
 * @details Suite de tests Qt Test couvrant toutes les méthodes publiques de AccessBDD.
 *          Chaque groupe de tests dispose de 2 à 3 cas. Une interface console interactive
 *          permet de saisir les paramètres de connexion et de choisir les tests à exécuter.
 */

#ifndef TESTCLASSBDD_H
#define TESTCLASSBDD_H

#include <QObject>
#include <QTest>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QTextStream>
#include <QDebug>
#include "accessbdd.h"

/**
 * @class TestClassBDD
 * @brief Classe de tests unitaires pour AccessBDD, conforme au framework Qt Test.
 *
 * Chaque méthode privée de type slot est détectée et exécutée automatiquement
 * par QTEST_MAIN. Les méthodes initTestCase / cleanupTestCase encadrent la suite
 * complète ; init / cleanup encadrent chaque test individuel.
 */
class TestClassBDD : public QObject
{
    Q_OBJECT

public:
    explicit TestClassBDD(QObject* parent = nullptr);

    // ── Paramètres de connexion saisis via la console ────────────────────────
    QString m_host     { "192.168.1.20" };
    QString m_base     { "DMXBDD" };
    QString m_user     { "root" };
    QString m_password { "raspberry" };

private:
    // Connexion de test dédiée (distincte de celle d'AccessBDD)
    QSqlDatabase m_db;

    // ── Helpers internes ─────────────────────────────────────────────────────
    /** Ouvre la connexion de test avec les paramètres fournis. */
    bool ouvrirConnexion();

    /** Supprime toutes les lignes de test créées pendant la suite. */
    void nettoyerDonneesDansTest();

    // ID trackés pour nettoyage automatique
    int m_idUniversTest   { -1 };
    int m_idEquipTest     { -1 };
    int m_idSceneTest     { -1 };

private slots:

    // ── Cycle de vie de la suite ──────────────────────────────────────────────
    void initTestCase();     ///< Exécuté UNE FOIS avant tous les tests
    void cleanupTestCase();  ///< Exécuté UNE FOIS après tous les tests
    void init();             ///< Exécuté avant CHAQUE test
    void cleanup();          ///< Exécuté après CHAQUE test

    // ── Tests : enregistrerUnivers ────────────────────────────────────────────
    /** CAS 1 : Insertion valide → true attendu */
    void test_enregistrerUnivers_valide();
    /** CAS 2 : IP vide → false attendu (validation interne) */
    void test_enregistrerUnivers_ipVide();
    /** CAS 3 : Numéro ≤ 0 → false attendu (validation interne) */
    void test_enregistrerUnivers_numeroNegatif();

    // ── Tests : chargerUnivers ────────────────────────────────────────────────
    /** CAS 1 : La liste contient au moins l'univers inséré en init */
    void test_chargerUnivers_nonVide();
    /** CAS 2 : Les données de l'univers inséré sont correctement lues */
    void test_chargerUnivers_donneesCorrectes();

    // ── Tests : modifierUnivers ───────────────────────────────────────────────
    /** CAS 1 : Modification d'un univers existant → true */
    void test_modifierUnivers_valide();
    /** CAS 2 : Modification avec ID inexistant → exec réussit mais rowsAffected=0 */
    void test_modifierUnivers_idInexistant();

    // ── Tests : supprimerUnivers ──────────────────────────────────────────────
    /** CAS 1 : Suppression d'un univers existant → true */
    void test_supprimerUnivers_valide();
    /** CAS 2 : Suppression d'un ID inexistant → exec réussit (0 lignes supprimées) */
    void test_supprimerUnivers_idInexistant();

    // ── Tests : enregistrerEquipment ──────────────────────────────────────────
    /** CAS 1 : Équipement valide avec canaux et fonctions → true */
    void test_enregistrerEquipment_valide();
    /** CAS 2 : idUnivers inexistant → false (contrainte FK) */
    void test_enregistrerEquipment_universInexistant();

    // ── Tests : modifierEquipment ─────────────────────────────────────────────
    /** CAS 1 : Modification d'un équipement existant → true */
    void test_modifierEquipment_valide();
    /** CAS 2 : ID d'équipement inexistant → false */
    void test_modifierEquipment_idInexistant();

    // ── Tests : supprimerEquipment ────────────────────────────────────────────
    /** CAS 1 : Suppression d'un équipement existant → true */
    void test_supprimerEquipment_valide();
    /** CAS 2 : ID inexistant → exec réussit mais sans effet */
    void test_supprimerEquipment_idInexistant();

    // ── Tests : chargerTousLesEquipements ─────────────────────────────────────
    /** CAS 1 : La liste n'est pas vide après insertion */
    void test_chargerTousLesEquipements_nonVide();
    /** CAS 2 : Les champs de l'équipement inséré sont correctement relus */
    void test_chargerTousLesEquipements_donneesCorrectes();

    // ── Tests : enregistrerScene ──────────────────────────────────────────────
    /** CAS 1 : Scène valide avec valeurs de canaux → true */
    void test_enregistrerScene_valide();
    /** CAS 2 : Scène avec map de canaux vide → true (aucun canal, scène vide) */
    void test_enregistrerScene_mapVide();
    /** CAS 3 : idCanal invalide → false (contrainte FK) */
    void test_enregistrerScene_canalInexistant();

    // ── Tests : chargerLesScenes ──────────────────────────────────────────────
    /** CAS 1 : La liste contient la scène insérée */
    void test_chargerLesScenes_nonVide();
    /** CAS 2 : Ordre alphabétique vérifié sur deux scènes */
    void test_chargerLesScenes_ordreAlphabetique();

    // ── Tests : chargerValeursScene ───────────────────────────────────────────
    /** CAS 1 : Les valeurs relues correspondent à celles insérées */
    void test_chargerValeursScene_donneesCorrectes();
    /** CAS 2 : ID de scène inexistant → map vide retournée */
    void test_chargerValeursScene_idInexistant();

    // ── Tests : renommerScene ─────────────────────────────────────────────────
    /** CAS 1 : Renommage valide → true et nom effectivement changé */
    void test_renommerScene_valide();
    /** CAS 2 : ID inexistant → exec réussit mais sans effet visible */
    void test_renommerScene_idInexistant();

    // ── Tests : supprimerScene ────────────────────────────────────────────────
    /** CAS 1 : Suppression d'une scène existante → true */
    void test_supprimerScene_valide();
    /** CAS 2 : ID inexistant → true (DELETE sans erreur SQL) */
    void test_supprimerScene_idInexistant();

    // ── Tests : recupererCompteurCanaux ──────────────────────────────────────
    /** CAS 1 : Compteur > 0 après insertion d'un équipement avec canaux */
    void test_recupererCompteurCanaux_nonZero();
    /** CAS 2 : Univers sans équipement → compteur = 0 */
    void test_recupererCompteurCanaux_zero();

    // ── Tests : getUniversDeScene ─────────────────────────────────────────────
    /** CAS 1 : Retourne le bon numéro d'univers pour une scène connue */
    void test_getUniversDeScene_valide();
    /** CAS 2 : ID de scène inconnu → retourne 1 (valeur par défaut) */
    void test_getUniversDeScene_idInexistant();

    // ── Tests : chargerMapUnivers ─────────────────────────────────────────────
    /** CAS 1 : La map contient les canaux de l'équipement inséré */
    void test_chargerMapUnivers_nonVide();
    /** CAS 2 : Univers inexistant → map vide */
    void test_chargerMapUnivers_universInexistant();
};

#endif // TESTCLASSBDD_H
