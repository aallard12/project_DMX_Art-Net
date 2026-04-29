/**
 * @file testclassbdd.cpp
 * @brief Implémentation des tests unitaires Qt Test pour AccessBDD
 * @author Antoine ALLARD
 * @date 10/04/2026
 */

#include "testclassbdd.h"
#include <QCoreApplication>
#include <QTextStream>
#include <QStringList>
#include <iostream>

// ═══════════════════════════════════════════════════════════════════════════════
//  Constructeur
// ═══════════════════════════════════════════════════════════════════════════════

TestClassBDD::TestClassBDD(QObject* parent)
    : QObject(parent)
{}

// ═══════════════════════════════════════════════════════════════════════════════
//  Helpers internes
// ═══════════════════════════════════════════════════════════════════════════════

bool TestClassBDD::ouvrirConnexion() {
    if (QSqlDatabase::contains("testConnection"))
        QSqlDatabase::removeDatabase("testConnection");

    m_db = QSqlDatabase::addDatabase("QMYSQL", "testConnection");
    m_db.setHostName(m_host);
    m_db.setDatabaseName(m_base);
    m_db.setUserName(m_user);
    m_db.setPassword(m_password);
    return m_db.open();
}

void TestClassBDD::nettoyerDonneesDansTest() {
    if (!m_db.isOpen()) return;

    if (m_idSceneTest > 0) {
        QSqlQuery q(m_db);
        q.prepare("DELETE FROM PILOTE WHERE idScene = :id");
        q.bindValue(":id", m_idSceneTest);
        q.exec();
        QSqlQuery q2(m_db);
        q2.prepare("DELETE FROM SCENES WHERE idScene = :id");
        q2.bindValue(":id", m_idSceneTest);
        q2.exec();
        m_idSceneTest = -1;
    }
    if (m_idEquipTest > 0) {
        QSqlQuery q(m_db);
        q.prepare("DELETE FROM EQUIPEMENTS WHERE idEquipement = :id");
        q.bindValue(":id", m_idEquipTest);
        q.exec();
        m_idEquipTest = -1;
    }
    if (m_idUniversTest > 0) {
        QSqlQuery q(m_db);
        q.prepare("DELETE FROM UNIVERS WHERE idUnivers = :id");
        q.bindValue(":id", m_idUniversTest);
        q.exec();
        m_idUniversTest = -1;
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Cycle de vie Qt Test
// ═══════════════════════════════════════════════════════════════════════════════

void TestClassBDD::initTestCase() {
    std::cout << "\n=== Initialisation de la suite de tests ===\n";
    QVERIFY2(ouvrirConnexion(), "Impossible d'ouvrir la connexion de vérification SQL directe.");
    std::cout << "✔ Connexion de verification etablie sur " << m_host.toStdString() << "/" << m_base.toStdString() << "\n";
}

void TestClassBDD::cleanupTestCase() {
    nettoyerDonneesDansTest();
    if (m_db.isOpen())
        m_db.close();
    QSqlDatabase::removeDatabase("testConnection");
    std::cout << "\n=== Suite de tests terminee - connexion fermee ===\n";
}

void TestClassBDD::init() {
    m_idUniversTest = -1;
    m_idEquipTest   = -1;
    m_idSceneTest   = -1;
}

void TestClassBDD::cleanup() {
    nettoyerDonneesDansTest();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  GROUPE 1 – enregistrerUnivers
// ═══════════════════════════════════════════════════════════════════════════════

void TestClassBDD::test_enregistrerUnivers_valide() {
    std::cout << "\n▶ [TEST] test_enregistrerUnivers_valide\n";
    int num = 99;
    QString ip = "10.0.0.99";
    std::cout << "  ├─ Valeurs preparees -> Numero : " << num << " | IP : " << ip.toStdString() << "\n";

    AccessBDD bdd;
    bool resultat = bdd.enregistrerUnivers(num, ip);
    std::cout << "  ├─ Resultat enregistrerUnivers : " << (resultat ? "TRUE" : "FALSE") << "\n";
    QVERIFY2(resultat, "L'enregistrement d'un univers valide doit retourner true.");

    QSqlQuery q(m_db);
    q.prepare("SELECT idUnivers FROM UNIVERS WHERE numeroUnivers = 99 AND adresseIp = '10.0.0.99'");
    q.exec();
    QVERIFY2(q.next(), "L'univers inséré doit être retrouvé en base.");
    m_idUniversTest = q.value(0).toInt();
    std::cout << "  └─ Verif BDD OK -> idUnivers recupere : " << m_idUniversTest << "\n";
}

void TestClassBDD::test_enregistrerUnivers_ipVide() {
    std::cout << "\n▶ [TEST] test_enregistrerUnivers_ipVide\n";
    AccessBDD bdd;
    std::cout << "  ├─ Tentative avec IP vide...\n";
    bool resultat = bdd.enregistrerUnivers(10, "   ");
    std::cout << "  └─ Resultat : " << (resultat ? "TRUE" : "FALSE") << " (attendu: FALSE)\n";
    QCOMPARE(resultat, false);
}

void TestClassBDD::test_enregistrerUnivers_numeroNegatif() {
    std::cout << "\n▶ [TEST] test_enregistrerUnivers_numeroNegatif\n";
    AccessBDD bdd;
    std::cout << "  ├─ Tentative avec Numero negatif (-5)...\n";
    bool resultat = bdd.enregistrerUnivers(-5, "192.168.0.1");
    std::cout << "  └─ Resultat : " << (resultat ? "TRUE" : "FALSE") << " (attendu: FALSE)\n";
    QCOMPARE(resultat, false);
}

// ═══════════════════════════════════════════════════════════════════════════════
//  GROUPE 2 – chargerUnivers
// ═══════════════════════════════════════════════════════════════════════════════

void TestClassBDD::test_chargerUnivers_nonVide() {
    std::cout << "\n▶ [TEST] test_chargerUnivers_nonVide\n";
    AccessBDD bdd;
    bdd.enregistrerUnivers(88, "10.0.0.88");

    QSqlQuery q(m_db);
    q.prepare("SELECT idUnivers FROM UNIVERS WHERE numeroUnivers = 88");
    q.exec(); q.next();
    m_idUniversTest = q.value(0).toInt();
    std::cout << "  ├─ Univers de test insere (ID: " << m_idUniversTest << ")\n";

    QList<UniversData> liste = bdd.chargerUnivers();
    std::cout << "  └─ Nombre d'univers charges : " << liste.size() << "\n";
    QVERIFY2(!liste.isEmpty(), "chargerUnivers doit retourner au moins un enregistrement.");
}

void TestClassBDD::test_chargerUnivers_donneesCorrectes() {
    std::cout << "\n▶ [TEST] test_chargerUnivers_donneesCorrectes\n";
    AccessBDD bdd;
    bdd.enregistrerUnivers(77, "10.0.0.77");

    QSqlQuery q(m_db);
    q.prepare("SELECT idUnivers FROM UNIVERS WHERE numeroUnivers = 77");
    q.exec(); q.next();
    m_idUniversTest = q.value(0).toInt();

    QList<UniversData> liste = bdd.chargerUnivers();
    bool trouve = false;
    for (const UniversData& u : liste) {
        if (u.numero == 77 && u.ip == "10.0.0.77") {
            trouve = true;
            std::cout << "  └─ Univers 77 trouve avec la bonne IP (" << u.ip.toStdString() << ")\n";
            break;
        }
    }
    QVERIFY2(trouve, "L'univers 77 avec l'IP 10.0.0.77 doit être présent dans la liste.");
}

// ═══════════════════════════════════════════════════════════════════════════════
//  GROUPE 3 – modifierUnivers
// ═══════════════════════════════════════════════════════════════════════════════

void TestClassBDD::test_modifierUnivers_valide() {
    std::cout << "\n▶ [TEST] test_modifierUnivers_valide\n";
    AccessBDD bdd;
    bdd.enregistrerUnivers(66, "10.0.0.66");

    QSqlQuery q(m_db);
    q.prepare("SELECT idUnivers FROM UNIVERS WHERE numeroUnivers = 66");
    q.exec(); q.next();
    m_idUniversTest = q.value(0).toInt();
    std::cout << "  ├─ Univers initial insere (ID: " << m_idUniversTest << ") | IP: 10.0.0.66\n";

    bool resultat = bdd.modifierUnivers(m_idUniversTest, 67, "10.0.0.67");
    std::cout << "  ├─ Modification en Numero: 67 | IP: 10.0.0.67 -> Resultat : " << (resultat ? "TRUE" : "FALSE") << "\n";
    QVERIFY2(resultat, "La modification d'un univers existant doit réussir.");

    QSqlQuery q2(m_db);
    q2.prepare("SELECT adresseIp FROM UNIVERS WHERE idUnivers = :id");
    q2.bindValue(":id", m_idUniversTest);
    q2.exec(); q2.next();
    std::cout << "  └─ Nouvelle IP lue en base : " << q2.value(0).toString().toStdString() << "\n";
    QCOMPARE(q2.value(0).toString(), QString("10.0.0.67"));
}

void TestClassBDD::test_modifierUnivers_idInexistant() {
    std::cout << "\n▶ [TEST] test_modifierUnivers_idInexistant\n";
    AccessBDD bdd;
    std::cout << "  ├─ Tentative de modification sur l'ID 999999...\n";
    bool resultat = bdd.modifierUnivers(999999, 1, "0.0.0.0");
    Q_UNUSED(resultat);

    QSqlQuery q(m_db);
    q.prepare("SELECT COUNT(*) FROM UNIVERS WHERE idUnivers = 999999");
    q.exec(); q.next();
    std::cout << "  └─ Lignes trouvees pour ID 999999 : " << q.value(0).toInt() << "\n";
    QCOMPARE(q.value(0).toInt(), 0);
}

// ═══════════════════════════════════════════════════════════════════════════════
//  GROUPE 4 – supprimerUnivers
// ═══════════════════════════════════════════════════════════════════════════════

void TestClassBDD::test_supprimerUnivers_valide() {
    std::cout << "\n▶ [TEST] test_supprimerUnivers_valide\n";
    AccessBDD bdd;
    bdd.enregistrerUnivers(55, "10.0.0.55");

    QSqlQuery q(m_db);
    q.prepare("SELECT idUnivers FROM UNIVERS WHERE numeroUnivers = 55");
    q.exec(); q.next();
    int id = q.value(0).toInt();
    m_idUniversTest = -1; // Desactive nettoyage auto
    std::cout << "  ├─ Univers a supprimer insere (ID: " << id << ")\n";

    bool resultat = bdd.supprimerUnivers(id);
    std::cout << "  ├─ Appel supprimerUnivers -> Resultat : " << (resultat ? "TRUE" : "FALSE") << "\n";
    QVERIFY2(resultat, "La suppression d'un univers existant doit réussir.");

    QSqlQuery q2(m_db);
    q2.prepare("SELECT COUNT(*) FROM UNIVERS WHERE idUnivers = :id");
    q2.bindValue(":id", id);
    q2.exec(); q2.next();
    std::cout << "  └─ Verif BDD : nb lignes restantes = " << q2.value(0).toInt() << "\n";
    QCOMPARE(q2.value(0).toInt(), 0);
}

void TestClassBDD::test_supprimerUnivers_idInexistant() {
    std::cout << "\n▶ [TEST] test_supprimerUnivers_idInexistant\n";
    AccessBDD bdd;
    bool resultat = bdd.supprimerUnivers(999998);
    std::cout << "  └─ Resultat suppression ID inexistant : " << (resultat ? "TRUE" : "FALSE") << "\n";
    QVERIFY2(resultat, "DELETE sur un ID inexistant ne doit pas générer d'erreur SQL.");
}

// ═══════════════════════════════════════════════════════════════════════════════
//  GROUPE 5 – enregistrerEquipment
// ═══════════════════════════════════════════════════════════════════════════════

void TestClassBDD::test_enregistrerEquipment_valide() {
    std::cout << "\n▶ [TEST] test_enregistrerEquipment_valide\n";
    AccessBDD bdd;
    bdd.enregistrerUnivers(44, "10.0.0.44");
    QSqlQuery q(m_db);
    q.prepare("SELECT idUnivers FROM UNIVERS WHERE numeroUnivers = 44");
    q.exec(); q.next();
    m_idUniversTest = q.value(0).toInt();

    EquipmentData eq;
    eq.nom      = "ProjecteurTest";
    eq.dmxStart = "1";
    eq.couleur  = "#FF0000";
    std::cout << "  ├─ Equipement prepare : " << eq.nom.toStdString() << " | Couleur: " << eq.couleur.toStdString() << " | Univers ID: " << m_idUniversTest << "\n";

    ChannelData chan;
    chan.description = "Intensite";
    FunctionData func;
    func.nom = "Full";
    func.min = "0";
    func.max = "255";
    chan.fonctions.append(func);
    eq.canaux.append(chan);

    bool resultat = bdd.enregistrerEquipment(eq, m_idUniversTest);
    std::cout << "  ├─ Resultat enregistrement : " << (resultat ? "TRUE" : "FALSE") << "\n";
    QVERIFY2(resultat, "L'enregistrement d'un équipement valide doit retourner true.");

    QSqlQuery q2(m_db);
    q2.prepare("SELECT idEquipement FROM EQUIPEMENTS WHERE nomEquipement = 'ProjecteurTest'");
    q2.exec(); q2.next();
    m_idEquipTest = q2.value(0).toInt();
    std::cout << "  └─ ID Equipement genere en base : " << m_idEquipTest << "\n";
}

void TestClassBDD::test_enregistrerEquipment_universInexistant() {
    std::cout << "\n▶ [TEST] test_enregistrerEquipment_universInexistant\n";
    AccessBDD bdd;
    EquipmentData eq;
    eq.nom = "EquipOrphan";
    eq.dmxStart = "1";
    eq.couleur = "#000000";

    std::cout << "  ├─ Tentative d'enregistrement sur idUnivers 999997...\n";
    bool resultat = bdd.enregistrerEquipment(eq, 999997);
    std::cout << "  └─ Resultat : " << (resultat ? "TRUE" : "FALSE") << " (attendu: FALSE)\n";
    QCOMPARE(resultat, false);
}

// ═══════════════════════════════════════════════════════════════════════════════
//  GROUPE 6 – modifierEquipment
// ═══════════════════════════════════════════════════════════════════════════════

void TestClassBDD::test_modifierEquipment_valide() {
    std::cout << "\n▶ [TEST] test_modifierEquipment_valide\n";
    AccessBDD bdd;
    bdd.enregistrerUnivers(33, "10.0.0.33");
    QSqlQuery q(m_db);
    q.prepare("SELECT idUnivers FROM UNIVERS WHERE numeroUnivers = 33");
    q.exec(); q.next();
    m_idUniversTest = q.value(0).toInt();

    EquipmentData eq;
    eq.nom = "EquipInitial"; eq.dmxStart = "1"; eq.couleur = "#00FF00";
    bdd.enregistrerEquipment(eq, m_idUniversTest);
    QSqlQuery q2(m_db);
    q2.prepare("SELECT idEquipement FROM EQUIPEMENTS WHERE nomEquipement = 'EquipInitial'");
    q2.exec(); q2.next();
    m_idEquipTest = q2.value(0).toInt();
    std::cout << "  ├─ Equipement insere : " << eq.nom.toStdString() << " (ID: " << m_idEquipTest << ")\n";

    EquipmentData eqModif;
    eqModif.nom = "EquipModifie"; eqModif.dmxStart = "5"; eqModif.couleur = "#0000FF";
    bool resultat = bdd.modifierEquipment(m_idEquipTest, eqModif, m_idUniversTest);
    std::cout << "  ├─ Modification en : " << eqModif.nom.toStdString() << " -> Resultat : " << (resultat ? "TRUE" : "FALSE") << "\n";
    QVERIFY2(resultat, "La modification d'un équipement existant doit réussir.");

    QSqlQuery q3(m_db);
    q3.prepare("SELECT nomEquipement FROM EQUIPEMENTS WHERE idEquipement = :id");
    q3.bindValue(":id", m_idEquipTest);
    q3.exec(); q3.next();
    std::cout << "  └─ Nouveau nom lu en BDD : " << q3.value(0).toString().toStdString() << "\n";
    QCOMPARE(q3.value(0).toString(), QString("EquipModifie"));
}

void TestClassBDD::test_modifierEquipment_idInexistant() {
    std::cout << "\n▶ [TEST] test_modifierEquipment_idInexistant\n";
    AccessBDD bdd;
    EquipmentData eq;
    eq.nom = "Fantome"; eq.dmxStart = "1"; eq.couleur = "#FFFFFF";
    bool resultat = bdd.modifierEquipment(999996, eq, 999995);
    std::cout << "  └─ Modification ID inexistant -> Resultat : " << (resultat ? "TRUE" : "FALSE") << " (attendu: FALSE)\n";
    QCOMPARE(resultat, false);
}

// ═══════════════════════════════════════════════════════════════════════════════
//  GROUPE 7 – supprimerEquipment
// ═══════════════════════════════════════════════════════════════════════════════

void TestClassBDD::test_supprimerEquipment_valide() {
    std::cout << "\n▶ [TEST] test_supprimerEquipment_valide\n";
    AccessBDD bdd;
    bdd.enregistrerUnivers(22, "10.0.0.22");
    QSqlQuery q(m_db);
    q.prepare("SELECT idUnivers FROM UNIVERS WHERE numeroUnivers = 22");
    q.exec(); q.next();
    m_idUniversTest = q.value(0).toInt();

    EquipmentData eq;
    eq.nom = "ASupprimer"; eq.dmxStart = "1"; eq.couleur = "#111111";
    bdd.enregistrerEquipment(eq, m_idUniversTest);
    QSqlQuery q2(m_db);
    q2.prepare("SELECT idEquipement FROM EQUIPEMENTS WHERE nomEquipement = 'ASupprimer'");
    q2.exec(); q2.next();
    int idEquip = q2.value(0).toInt();
    m_idEquipTest = -1;
    std::cout << "  ├─ Equipement a supprimer insere (ID: " << idEquip << ")\n";

    bool resultat = bdd.supprimerEquipment(idEquip);
    std::cout << "  ├─ Resultat suppression : " << (resultat ? "TRUE" : "FALSE") << "\n";
    QVERIFY2(resultat, "La suppression d'un équipement existant doit réussir.");

    QSqlQuery q3(m_db);
    q3.prepare("SELECT COUNT(*) FROM EQUIPEMENTS WHERE idEquipement = :id");
    q3.bindValue(":id", idEquip);
    q3.exec(); q3.next();
    std::cout << "  └─ Verif BDD : nb lignes restantes = " << q3.value(0).toInt() << "\n";
    QCOMPARE(q3.value(0).toInt(), 0);
}

void TestClassBDD::test_supprimerEquipment_idInexistant() {
    std::cout << "\n▶ [TEST] test_supprimerEquipment_idInexistant\n";
    AccessBDD bdd;
    bool resultat = bdd.supprimerEquipment(999994);
    std::cout << "  └─ Resultat suppression ID inexistant : " << (resultat ? "TRUE" : "FALSE") << "\n";
    QVERIFY2(resultat, "DELETE sur un ID équipement inexistant ne doit pas générer d'erreur SQL.");
}

// ═══════════════════════════════════════════════════════════════════════════════
//  GROUPE 8 – chargerTousLesEquipements
// ═══════════════════════════════════════════════════════════════════════════════

void TestClassBDD::test_chargerTousLesEquipements_nonVide() {
    std::cout << "\n▶ [TEST] test_chargerTousLesEquipements_nonVide\n";
    AccessBDD bdd;
    bdd.enregistrerUnivers(11, "10.0.0.11");
    QSqlQuery q(m_db);
    q.prepare("SELECT idUnivers FROM UNIVERS WHERE numeroUnivers = 11");
    q.exec(); q.next();
    m_idUniversTest = q.value(0).toInt();

    EquipmentData eq;
    eq.nom = "EquipLecture"; eq.dmxStart = "1"; eq.couleur = "#222222";
    bdd.enregistrerEquipment(eq, m_idUniversTest);
    QSqlQuery q2(m_db);
    q2.prepare("SELECT idEquipement FROM EQUIPEMENTS WHERE nomEquipement = 'EquipLecture'");
    q2.exec(); q2.next();
    m_idEquipTest = q2.value(0).toInt();
    std::cout << "  ├─ Equipement insere pour lecture\n";

    QList<EquipmentData> liste = bdd.chargerTousLesEquipements();
    std::cout << "  └─ Nombre d'equipements charges : " << liste.size() << "\n";
    QVERIFY2(!liste.isEmpty(), "chargerTousLesEquipements ne doit pas retourner une liste vide.");
}

void TestClassBDD::test_chargerTousLesEquipements_donneesCorrectes() {
    std::cout << "\n▶ [TEST] test_chargerTousLesEquipements_donneesCorrectes\n";
    AccessBDD bdd;
    bdd.enregistrerUnivers(10, "10.0.0.10");
    QSqlQuery q(m_db);
    q.prepare("SELECT idUnivers FROM UNIVERS WHERE numeroUnivers = 10");
    q.exec(); q.next();
    m_idUniversTest = q.value(0).toInt();

    EquipmentData eqRef;
    eqRef.nom = "EquipDonnees"; eqRef.dmxStart = "5"; eqRef.couleur = "#333333";
    bdd.enregistrerEquipment(eqRef, m_idUniversTest);
    QSqlQuery q2(m_db);
    q2.prepare("SELECT idEquipement FROM EQUIPEMENTS WHERE nomEquipement = 'EquipDonnees'");
    q2.exec(); q2.next();
    m_idEquipTest = q2.value(0).toInt();

    QList<EquipmentData> liste = bdd.chargerTousLesEquipements();
    bool trouve = false;
    for (const EquipmentData& e : liste) {
        if (e.nom == "EquipDonnees" && e.couleur == "#333333") {
            trouve = true;
            std::cout << "  └─ Equipement trouve avec la bonne couleur (" << e.couleur.toStdString() << ")\n";
            break;
        }
    }
    QVERIFY2(trouve, "L'équipement 'EquipDonnees' doit être retrouvé avec la bonne couleur.");
}

// ═══════════════════════════════════════════════════════════════════════════════
//  GROUPE 9 – enregistrerScene
// ═══════════════════════════════════════════════════════════════════════════════

static int creerCanalDeTest(QSqlDatabase& db, AccessBDD& bdd,
                            int& outIdUnivers, int& outIdEquip,
                            int numUnivers, const QString& ip) {
    bdd.enregistrerUnivers(numUnivers, ip);
    QSqlQuery q(db);
    q.prepare("SELECT idUnivers FROM UNIVERS WHERE numeroUnivers = :n");
    q.bindValue(":n", numUnivers); q.exec(); q.next();
    outIdUnivers = q.value(0).toInt();

    EquipmentData eq;
    eq.nom = "SceneSupport"; eq.dmxStart = "1"; eq.couleur = "#AAAAAA";
    ChannelData chan; chan.description = "Master";
    eq.canaux.append(chan);
    bdd.enregistrerEquipment(eq, outIdUnivers);

    QSqlQuery q2(db);
    q2.prepare("SELECT idEquipement FROM EQUIPEMENTS WHERE nomEquipement = 'SceneSupport'");
    q2.exec(); q2.next();
    outIdEquip = q2.value(0).toInt();

    QSqlQuery q3(db);
    q3.prepare("SELECT idCanal FROM CANAUX WHERE idEquipement = :id LIMIT 1");
    q3.bindValue(":id", outIdEquip); q3.exec(); q3.next();
    return q3.value(0).toInt();
}

void TestClassBDD::test_enregistrerScene_valide() {
    std::cout << "\n▶ [TEST] test_enregistrerScene_valide\n";
    AccessBDD bdd;
    int idCanal = creerCanalDeTest(m_db, bdd, m_idUniversTest, m_idEquipTest, 201, "10.1.0.1");
    std::cout << "  ├─ Creation canal de support OK (ID: " << idCanal << ")\n";

    QMap<int,int> valeurs;
    valeurs.insert(idCanal, 200);
    bool resultat = bdd.enregistrerScene("SceneTestValide", valeurs);
    std::cout << "  ├─ Enregistrement de la scene -> Resultat : " << (resultat ? "TRUE" : "FALSE") << "\n";
    QVERIFY2(resultat, "L'enregistrement d'une scène valide doit retourner true.");

    QSqlQuery q(m_db);
    q.prepare("SELECT idScene FROM SCENES WHERE nomScene = 'SceneTestValide'");
    q.exec(); q.next();
    m_idSceneTest = q.value(0).toInt();
    std::cout << "  └─ ID Scene genere en base : " << m_idSceneTest << "\n";
}

void TestClassBDD::test_enregistrerScene_mapVide() {
    std::cout << "\n▶ [TEST] test_enregistrerScene_mapVide\n";
    AccessBDD bdd;
    QMap<int,int> vide;
    std::cout << "  ├─ Tentative d'enregistrement avec map vide...\n";
    bool resultat = bdd.enregistrerScene("SceneVide", vide);
    std::cout << "  └─ Resultat : " << (resultat ? "TRUE" : "FALSE") << "\n";
    QVERIFY2(resultat, "Une scène sans canaux doit tout de même s'enregistrer.");

    QSqlQuery q(m_db);
    q.prepare("SELECT idScene FROM SCENES WHERE nomScene = 'SceneVide'");
    q.exec(); q.next();
    m_idSceneTest = q.value(0).toInt();
}

void TestClassBDD::test_enregistrerScene_canalInexistant() {
    std::cout << "\n▶ [TEST] test_enregistrerScene_canalInexistant\n";
    AccessBDD bdd;
    QMap<int,int> valeurs;
    valeurs.insert(999993, 128);
    std::cout << "  ├─ Tentative d'enregistrement sur idCanal inexistant (999993)...\n";
    bool resultat = bdd.enregistrerScene("SceneCanalFK", valeurs);
    std::cout << "  └─ Resultat : " << (resultat ? "TRUE" : "FALSE") << " (attendu: FALSE)\n";
    QCOMPARE(resultat, false);
}

// ═══════════════════════════════════════════════════════════════════════════════
//  GROUPE 10 – chargerLesScenes
// ═══════════════════════════════════════════════════════════════════════════════

void TestClassBDD::test_chargerLesScenes_nonVide() {
    std::cout << "\n▶ [TEST] test_chargerLesScenes_nonVide\n";
    AccessBDD bdd;
    QMap<int,int> vide;
    bdd.enregistrerScene("SceneLectureA", vide);
    QSqlQuery q(m_db);
    q.prepare("SELECT idScene FROM SCENES WHERE nomScene = 'SceneLectureA'");
    q.exec(); q.next();
    m_idSceneTest = q.value(0).toInt();

    QList<SceneData> liste = bdd.chargerLesScenes();
    std::cout << "  └─ Nombre de scenes chargees : " << liste.size() << "\n";
    QVERIFY2(!liste.isEmpty(), "chargerLesScenes ne doit pas retourner de liste vide.");
}

void TestClassBDD::test_chargerLesScenes_ordreAlphabetique() {
    std::cout << "\n▶ [TEST] test_chargerLesScenes_ordreAlphabetique\n";
    AccessBDD bdd;
    QMap<int,int> vide;
    bdd.enregistrerScene("ZZZ_SceneOrdre", vide);
    bdd.enregistrerScene("AAA_SceneOrdre", vide);

    QSqlQuery q(m_db);
    q.prepare("SELECT idScene FROM SCENES WHERE nomScene = 'ZZZ_SceneOrdre'");
    q.exec(); q.next();
    int idZZZ = q.value(0).toInt();
    QSqlQuery q2(m_db);
    q2.prepare("SELECT idScene FROM SCENES WHERE nomScene = 'AAA_SceneOrdre'");
    q2.exec(); q2.next();
    int idAAA = q2.value(0).toInt();
    std::cout << "  ├─ Scenes AAA et ZZZ inserees\n";

    QList<SceneData> liste = bdd.chargerLesScenes();
    int posAAA = -1, posZZZ = -1;
    for (int i = 0; i < liste.size(); ++i) {
        if (liste[i].idScene == idAAA) posAAA = i;
        if (liste[i].idScene == idZZZ) posZZZ = i;
    }
    std::cout << "  └─ Positions trouvees -> AAA : " << posAAA << " | ZZZ : " << posZZZ << "\n";
    QVERIFY2(posAAA < posZZZ, "Les scènes doivent être triées par ordre alphabétique ascendant.");

    QSqlQuery del(m_db);
    del.prepare("DELETE FROM SCENES WHERE idScene IN (:a, :z)");
    del.bindValue(":a", idAAA); del.bindValue(":z", idZZZ);
    del.exec();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  GROUPE 11 – chargerValeursScene
// ═══════════════════════════════════════════════════════════════════════════════

void TestClassBDD::test_chargerValeursScene_donneesCorrectes() {
    std::cout << "\n▶ [TEST] test_chargerValeursScene_donneesCorrectes\n";
    AccessBDD bdd;
    int idCanal = creerCanalDeTest(m_db, bdd, m_idUniversTest, m_idEquipTest, 202, "10.2.0.1");

    QMap<int,int> valeurs;
    valeurs.insert(idCanal, 128);
    bdd.enregistrerScene("SceneValeurs", valeurs);
    QSqlQuery q(m_db);
    q.prepare("SELECT idScene FROM SCENES WHERE nomScene = 'SceneValeurs'");
    q.exec(); q.next();
    m_idSceneTest = q.value(0).toInt();
    std::cout << "  ├─ Scene inseree avec Canal ID: " << idCanal << " -> Valeur: 128\n";

    QMap<int,int> lu = bdd.chargerValeursScene(m_idSceneTest);
    std::cout << "  └─ Lecture depuis la BDD -> Valeur lue: " << lu.value(idCanal, -1) << "\n";
    QVERIFY2(lu.contains(idCanal), "La map doit contenir l'idCanal inséré.");
    QCOMPARE(lu.value(idCanal), 128);
}

void TestClassBDD::test_chargerValeursScene_idInexistant() {
    std::cout << "\n▶ [TEST] test_chargerValeursScene_idInexistant\n";
    AccessBDD bdd;
    QMap<int,int> lu = bdd.chargerValeursScene(999992);
    std::cout << "  └─ Lecture sur ID inexistant -> Map est vide : " << (lu.isEmpty() ? "OUI" : "NON") << "\n";
    QVERIFY2(lu.isEmpty(), "Une scène inexistante doit retourner une map vide.");
}

// ═══════════════════════════════════════════════════════════════════════════════
//  GROUPE 12 – renommerScene
// ═══════════════════════════════════════════════════════════════════════════════

void TestClassBDD::test_renommerScene_valide() {
    std::cout << "\n▶ [TEST] test_renommerScene_valide\n";
    AccessBDD bdd;
    QMap<int,int> vide;
    bdd.enregistrerScene("SceneAvantRenom", vide);
    QSqlQuery q(m_db);
    q.prepare("SELECT idScene FROM SCENES WHERE nomScene = 'SceneAvantRenom'");
    q.exec(); q.next();
    m_idSceneTest = q.value(0).toInt();

    std::cout << "  ├─ Scene inseree : SceneAvantRenom (ID: " << m_idSceneTest << ")\n";
    bool resultat = bdd.renommerScene(m_idSceneTest, "SceneApresRenom");
    std::cout << "  ├─ Appel renommerScene -> Resultat : " << (resultat ? "TRUE" : "FALSE") << "\n";
    QVERIFY2(resultat, "Le renommage d'une scène existante doit réussir.");

    QSqlQuery q2(m_db);
    q2.prepare("SELECT nomScene FROM SCENES WHERE idScene = :id");
    q2.bindValue(":id", m_idSceneTest);
    q2.exec(); q2.next();
    std::cout << "  └─ Nouveau nom lu en BDD : " << q2.value(0).toString().toStdString() << "\n";
    QCOMPARE(q2.value(0).toString(), QString("SceneApresRenom"));
}

void TestClassBDD::test_renommerScene_idInexistant() {
    std::cout << "\n▶ [TEST] test_renommerScene_idInexistant\n";
    AccessBDD bdd;
    bool resultat = bdd.renommerScene(999991, "NomFantome");
    Q_UNUSED(resultat);

    QSqlQuery q(m_db);
    q.prepare("SELECT COUNT(*) FROM SCENES WHERE nomScene = 'NomFantome'");
    q.exec(); q.next();
    std::cout << "  └─ Lignes trouvees avec NomFantome : " << q.value(0).toInt() << "\n";
    QCOMPARE(q.value(0).toInt(), 0);
}

// ═══════════════════════════════════════════════════════════════════════════════
//  GROUPE 13 – supprimerScene
// ═══════════════════════════════════════════════════════════════════════════════

void TestClassBDD::test_supprimerScene_valide() {
    std::cout << "\n▶ [TEST] test_supprimerScene_valide\n";
    AccessBDD bdd;
    QMap<int,int> vide;
    bdd.enregistrerScene("SceneASupprimer", vide);
    QSqlQuery q(m_db);
    q.prepare("SELECT idScene FROM SCENES WHERE nomScene = 'SceneASupprimer'");
    q.exec(); q.next();
    int id = q.value(0).toInt();
    m_idSceneTest = -1; // Supprimé dans le test lui-même

    std::cout << "  ├─ Scene a supprimer inseree (ID: " << id << ")\n";
    bool resultat = bdd.supprimerScene(id);
    std::cout << "  ├─ Appel supprimerScene -> Resultat : " << (resultat ? "TRUE" : "FALSE") << "\n";
    QVERIFY2(resultat, "La suppression d'une scène existante doit réussir.");

    QSqlQuery q2(m_db);
    q2.prepare("SELECT COUNT(*) FROM SCENES WHERE idScene = :id");
    q2.bindValue(":id", id); q2.exec(); q2.next();
    std::cout << "  └─ Verif BDD : nb lignes restantes = " << q2.value(0).toInt() << "\n";
    QCOMPARE(q2.value(0).toInt(), 0);
}

void TestClassBDD::test_supprimerScene_idInexistant() {
    std::cout << "\n▶ [TEST] test_supprimerScene_idInexistant\n";
    AccessBDD bdd;
    bool resultat = bdd.supprimerScene(999990);
    std::cout << "  └─ Resultat suppression ID inexistant : " << (resultat ? "TRUE" : "FALSE") << "\n";
    QVERIFY2(resultat, "DELETE sur un idScene inexistant ne doit pas générer d'erreur SQL.");
}

// ═══════════════════════════════════════════════════════════════════════════════
//  GROUPE 14 – recupererCompteurCanaux
// ═══════════════════════════════════════════════════════════════════════════════

void TestClassBDD::test_recupererCompteurCanaux_nonZero() {
    std::cout << "\n▶ [TEST] test_recupererCompteurCanaux_nonZero\n";
    AccessBDD bdd;
    bdd.enregistrerUnivers(301, "10.3.0.1");
    QSqlQuery q(m_db);
    q.prepare("SELECT idUnivers FROM UNIVERS WHERE numeroUnivers = 301");
    q.exec(); q.next();
    m_idUniversTest = q.value(0).toInt();

    EquipmentData eq;
    eq.nom = "EquipCompteur"; eq.dmxStart = "1"; eq.couleur = "#CCCCCC";
    ChannelData c1; c1.description = "Ch1";
    ChannelData c2; c2.description = "Ch2";
    eq.canaux.append(c1); eq.canaux.append(c2);
    bdd.enregistrerEquipment(eq, m_idUniversTest);
    QSqlQuery q2(m_db);
    q2.prepare("SELECT idEquipement FROM EQUIPEMENTS WHERE nomEquipement = 'EquipCompteur'");
    q2.exec(); q2.next();
    m_idEquipTest = q2.value(0).toInt();

    std::cout << "  ├─ Equipement insere avec 2 canaux\n";
    int compteur = bdd.recupererCompteurCanaux(m_idUniversTest);
    std::cout << "  └─ Compteur lu : " << compteur << "\n";
    QVERIFY2(compteur >= 2, "Le compteur doit refléter les 2 canaux insérés.");
}

void TestClassBDD::test_recupererCompteurCanaux_zero() {
    std::cout << "\n▶ [TEST] test_recupererCompteurCanaux_zero\n";
    AccessBDD bdd;
    bdd.enregistrerUnivers(302, "10.3.0.2");
    QSqlQuery q(m_db);
    q.prepare("SELECT idUnivers FROM UNIVERS WHERE numeroUnivers = 302");
    q.exec(); q.next();
    m_idUniversTest = q.value(0).toInt();

    std::cout << "  ├─ Univers sans equipement insere\n";
    int compteur = bdd.recupererCompteurCanaux(m_idUniversTest);
    std::cout << "  └─ Compteur lu : " << compteur << " (attendu: 0)\n";
    QCOMPARE(compteur, 0);
}

// ═══════════════════════════════════════════════════════════════════════════════
//  GROUPE 15 – getUniversDeScene
// ═══════════════════════════════════════════════════════════════════════════════

void TestClassBDD::test_getUniversDeScene_valide() {
    std::cout << "\n▶ [TEST] test_getUniversDeScene_valide\n";
    AccessBDD bdd;
    int idCanal = creerCanalDeTest(m_db, bdd, m_idUniversTest, m_idEquipTest, 401, "10.4.0.1");

    QSqlQuery qu(m_db);
    qu.prepare("SELECT numeroUnivers FROM UNIVERS WHERE idUnivers = :id");
    qu.bindValue(":id", m_idUniversTest); qu.exec(); qu.next();
    int numUniversAttendu = qu.value(0).toInt();

    QMap<int,int> valeurs;
    valeurs.insert(idCanal, 100);
    bdd.enregistrerScene("SceneUniversTest", valeurs);
    QSqlQuery qs(m_db);
    qs.prepare("SELECT idScene FROM SCENES WHERE nomScene = 'SceneUniversTest'");
    qs.exec(); qs.next();
    m_idSceneTest = qs.value(0).toInt();

    std::cout << "  ├─ Scene inseree pour l'Univers N° " << numUniversAttendu << "\n";

    int numUnivers = bdd.getUniversDeScene(m_idSceneTest);
    std::cout << "  └─ Numero d'univers recupere par la fonction : " << numUnivers << "\n";
    QCOMPARE(numUnivers, numUniversAttendu);
}

void TestClassBDD::test_getUniversDeScene_idInexistant() {
    std::cout << "\n▶ [TEST] test_getUniversDeScene_idInexistant\n";
    AccessBDD bdd;
    int resultat = bdd.getUniversDeScene(999989);
    std::cout << "  └─ Lecture sur ID scene inexistant -> Resultat : " << resultat << " (attendu: 1 par defaut)\n";
    QCOMPARE(resultat, 1);
}

// ═══════════════════════════════════════════════════════════════════════════════
//  GROUPE 16 – chargerMapUnivers
// ═══════════════════════════════════════════════════════════════════════════════

void TestClassBDD::test_chargerMapUnivers_nonVide() {
    std::cout << "\n▶ [TEST] test_chargerMapUnivers_nonVide\n";
    AccessBDD bdd;
    bdd.enregistrerUnivers(501, "10.5.0.1");
    QSqlQuery q(m_db);
    q.prepare("SELECT idUnivers FROM UNIVERS WHERE numeroUnivers = 501");
    q.exec(); q.next();
    m_idUniversTest = q.value(0).toInt();

    EquipmentData eq;
    eq.nom = "EquipMap"; eq.dmxStart = "1"; eq.couleur = "#DDDDDD";
    ChannelData chan; chan.description = "Pan";
    eq.canaux.append(chan);
    bdd.enregistrerEquipment(eq, m_idUniversTest);
    QSqlQuery q2(m_db);
    q2.prepare("SELECT idEquipement FROM EQUIPEMENTS WHERE nomEquipement = 'EquipMap'");
    q2.exec(); q2.next();
    m_idEquipTest = q2.value(0).toInt();

    std::cout << "  ├─ Equipement insere dans l'univers ID: " << m_idUniversTest << "\n";
    QMap<int, DmxChannelInfo> map = bdd.chargerMapUnivers(m_idUniversTest);
    std::cout << "  └─ Nombre d'elements dans la Map DMX chargee : " << map.size() << "\n";
    QVERIFY2(!map.isEmpty(), "chargerMapUnivers doit retourner au moins un canal.");
}

void TestClassBDD::test_chargerMapUnivers_universInexistant() {
    std::cout << "\n▶ [TEST] test_chargerMapUnivers_universInexistant\n";
    AccessBDD bdd;
    QMap<int, DmxChannelInfo> map = bdd.chargerMapUnivers(999988);
    std::cout << "  └─ Chargement Map sur Univers inexistant -> Map est vide : " << (map.isEmpty() ? "OUI" : "NON") << "\n";
    QVERIFY2(map.isEmpty(), "Un univers inexistant doit retourner une map vide.");
}
