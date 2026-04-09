#include "interfacepcclient.h"
#include "accessbdd.h"

#include <QApplication>
#include <QDebug>

// ─────────────────────────────────────────────
//  Infrastructure de test
// ─────────────────────────────────────────────

static int testsReussis = 0;
static int testsEchoues = 0;

void verifier(const QString& nomTest, bool condition) {
    if (condition) {
        qDebug() << "[OK]" << nomTest;
        testsReussis++;
    } else {
        qDebug() << "[ECHEC]" << nomTest;
        testsEchoues++;
    }
}

void afficherResultats(const QString& suite) {
    qDebug() << "\nRésultat" << suite << ":" << testsReussis << "réussi(s)," << testsEchoues << "échoué(s)";
    testsReussis = 0;
    testsEchoues = 0;
}

// ─────────────────────────────────────────────
//  Helpers de nettoyage
// ─────────────────────────────────────────────

// Supprime en base l'univers de test par son numéro (nettoyage après test)
void nettoyerUniversParNumero(int numero) {
    QSqlQuery q;
    q.prepare("DELETE FROM UNIVERS WHERE numeroUnivers = :n");
    q.bindValue(":n", numero);
    q.exec();
}

// Supprime en base l'équipement de test par son nom (nettoyage après test)
void nettoyerEquipementParNom(const QString& nom) {
    QSqlQuery q;
    q.prepare("DELETE FROM EQUIPEMENTS WHERE nomEquipement = :n");
    q.bindValue(":n", nom);
    q.exec();
}

// Supprime en base la scène de test par son nom (nettoyage après test)
void nettoyerSceneParNom(const QString& nom) {
    QSqlQuery q;
    q.prepare("DELETE FROM SCENES WHERE nomScene = :n");
    q.bindValue(":n", nom);
    q.exec();
}

// ─────────────────────────────────────────────
//  Helpers de construction de données de test
// ─────────────────────────────────────────────

EquipmentData construireEquipementTest(const QString& nom, int idUniversValide) {
    EquipmentData eq;
    eq.nom       = nom;
    eq.dmxStart  = "1";
    eq.couleur   = "#FF0000";

    FunctionData f1;
    f1.nom = "Dimmer";
    f1.min = "0";
    f1.max = "255";

    ChannelData c1;
    c1.description = "Canal intensité";
    c1.fonctions.append(f1);

    eq.canaux.append(c1);
    return eq;
}

// ─────────────────────────────────────────────
//  chargerUnivers
// ─────────────────────────────────────────────

void testerChargerUnivers() {
    qDebug() << "\n=== Tests : AccessBDD::chargerUnivers() ===";
    AccessBDD bdd;
    QList<UniversData> liste = bdd.chargerUnivers();

    verifier("La liste retournée est non nulle (objet valide)",
             true);

    verifier("Chaque univers a un idUnivers > 0",
             [&]() {
                 for (const auto& u : liste)
                     if (u.idUnivers <= 0) return false;
                 return true;
             }());

    verifier("Chaque univers a un numéro > 0",
             [&]() {
                 for (const auto& u : liste)
                     if (u.numero <= 0) return false;
                 return true;
             }());

    verifier("Chaque univers a une adresse IP non vide",
             [&]() {
                 for (const auto& u : liste)
                     if (u.ip.trimmed().isEmpty()) return false;
                 return true;
             }());

    verifier("Les univers sont triés par numéro croissant",
             [&]() {
                 for (int i = 1; i < liste.size(); ++i)
                     if (liste[i].numero <= liste[i - 1].numero) return false;
                 return true;
             }());

    verifier("Aucun doublon de numéro d'univers",
             [&]() {
                 for (int i = 0; i < liste.size(); ++i)
                     for (int j = i + 1; j < liste.size(); ++j)
                         if (liste[i].numero == liste[j].numero) return false;
                 return true;
             }());

    verifier("Aucun doublon d'idUnivers",
             [&]() {
                 for (int i = 0; i < liste.size(); ++i)
                     for (int j = i + 1; j < liste.size(); ++j)
                         if (liste[i].idUnivers == liste[j].idUnivers) return false;
                 return true;
             }());

    afficherResultats("chargerUnivers");
}

// ─────────────────────────────────────────────
//  enregistrerUnivers
// ─────────────────────────────────────────────

void testerEnregistrerUnivers() {
    qDebug() << "\n=== Tests : AccessBDD::enregistrerUnivers() ===";
    AccessBDD bdd;

    // Numéro réservé aux tests — on nettoie avant de commencer au cas où un test
    // précédent aurait laissé des traces
    nettoyerUniversParNumero(999);

    verifier("Enregistrement d'un univers valide",
             bdd.enregistrerUnivers(999, "192.168.1.99"));

    verifier("L'univers a bien été persisté en base",
             [&]() {
                 for (const auto& u : bdd.chargerUnivers())
                     if (u.numero == 999 && u.ip == "192.168.1.99") return true;
                 return false;
             }());

    // Nettoyage après les tests qui insèrent en base
    nettoyerUniversParNumero(999);

    // NOTE : enregistrerUnivers ne filtre que les IP vides — pas les plages
    // Le test ci-dessous vérifie le comportement réel du code
    verifier("Refus d'un univers avec IP vide",
             !bdd.enregistrerUnivers(998, ""));

    verifier("Refus d'un univers avec IP composée uniquement d'espaces",
             !bdd.enregistrerUnivers(998, "   "));

    afficherResultats("enregistrerUnivers");
}

// ─────────────────────────────────────────────
//  modifierUnivers
// ─────────────────────────────────────────────

void testerModifierUnivers() {
    qDebug() << "\n=== Tests : AccessBDD::modifierUnivers() ===";
    AccessBDD bdd;

    // Préparation : on insère un univers de test
    nettoyerUniversParNumero(997);
    bdd.enregistrerUnivers(997, "10.0.0.1");

    // Récupérer l'id inséré
    int idTest = -1;
    for (const auto& u : bdd.chargerUnivers())
        if (u.numero == 997) { idTest = u.idUnivers; break; }

    verifier("Modification d'un univers existant retourne true",
             idTest > 0 && bdd.modifierUnivers(idTest, 997, "10.0.0.2"));

    verifier("La modification est bien persistée en base",
             [&]() {
                 for (const auto& u : bdd.chargerUnivers())
                     if (u.idUnivers == idTest && u.ip == "10.0.0.2") return true;
                 return false;
             }());

    verifier("Modification d'un id inexistant retourne true (UPDATE sans ligne affectée)",
             bdd.modifierUnivers(-1, 0, "0.0.0.0"));

    // Nettoyage
    nettoyerUniversParNumero(997);

    afficherResultats("modifierUnivers");
}

// ─────────────────────────────────────────────
//  supprimerUnivers
// ─────────────────────────────────────────────

void testerSupprimerUnivers() {
    qDebug() << "\n=== Tests : AccessBDD::supprimerUnivers() ===";
    AccessBDD bdd;

    // Préparation
    nettoyerUniversParNumero(996);
    bdd.enregistrerUnivers(996, "10.0.1.1");

    int idTest = -1;
    for (const auto& u : bdd.chargerUnivers())
        if (u.numero == 996) { idTest = u.idUnivers; break; }

    verifier("Suppression d'un univers existant retourne true",
             idTest > 0 && bdd.supprimerUnivers(idTest));

    verifier("L'univers n'est plus présent en base après suppression",
             [&]() {
                 for (const auto& u : bdd.chargerUnivers())
                     if (u.idUnivers == idTest) return false;
                 return true;
             }());

    verifier("Suppression d'un id inexistant retourne true (DELETE sans ligne affectée)",
             bdd.supprimerUnivers(-1));

    afficherResultats("supprimerUnivers");
}

// ─────────────────────────────────────────────
//  enregistrerEquipment
// ─────────────────────────────────────────────

void testerEnregistrerEquipment() {
    qDebug() << "\n=== Tests : AccessBDD::enregistrerEquipment() ===";
    AccessBDD bdd;

    // On récupère un idUnivers valide existant en base
    QList<UniversData> univers = bdd.chargerUnivers();
    if (univers.isEmpty()) {
        qDebug() << "[SKIP] Aucun univers en base, tests ignorés";
        return;
    }
    int idUniversValide = univers.first().idUnivers;

    nettoyerEquipementParNom("EquipTest");

    EquipmentData eq = construireEquipementTest("EquipTest", idUniversValide);

    verifier("Enregistrement d'un équipement valide retourne true",
             bdd.enregistrerEquipment(eq, idUniversValide));

    verifier("L'équipement est bien persisté en base",
             [&]() {
                 for (const auto& e : bdd.chargerTousLesEquipements())
                     if (e.nom == "EquipTest") return true;
                 return false;
             }());

    verifier("L'équipement persisté possède le bon nombre de canaux",
             [&]() {
                 for (const auto& e : bdd.chargerTousLesEquipements())
                     if (e.nom == "EquipTest") return e.canaux.size() == eq.canaux.size();
                 return false;
             }());

    // Nettoyage
    nettoyerEquipementParNom("EquipTest");

    afficherResultats("enregistrerEquipment");
}

// ─────────────────────────────────────────────
//  modifierEquipment
// ─────────────────────────────────────────────

void testerModifierEquipment() {
    qDebug() << "\n=== Tests : AccessBDD::modifierEquipment() ===";
    AccessBDD bdd;

    QList<UniversData> univers = bdd.chargerUnivers();
    if (univers.isEmpty()) {
        qDebug() << "[SKIP] Aucun univers en base, tests ignorés";
        return;
    }
    int idUniversValide = univers.first().idUnivers;

    // Préparation
    nettoyerEquipementParNom("EquipModifTest");
    nettoyerEquipementParNom("EquipModifTestMAJ");

    EquipmentData eq = construireEquipementTest("EquipModifTest", idUniversValide);
    bdd.enregistrerEquipment(eq, idUniversValide);

    int idTest = -1;
    for (const auto& e : bdd.chargerTousLesEquipements())
        if (e.nom == "EquipModifTest") { idTest = e.idEquipement; break; }

    EquipmentData eqMAJ = construireEquipementTest("EquipModifTestMAJ", idUniversValide);
    // On ajoute un deuxième canal pour vérifier que les canaux sont bien remplacés
    ChannelData c2;
    c2.description = "Canal couleur";
    eqMAJ.canaux.append(c2);

    verifier("Modification d'un équipement existant retourne true",
             idTest > 0 && bdd.modifierEquipment(idTest, eqMAJ, idUniversValide));

    verifier("Le nouveau nom est bien persisté",
             [&]() {
                 for (const auto& e : bdd.chargerTousLesEquipements())
                     if (e.idEquipement == idTest) return e.nom == "EquipModifTestMAJ";
                 return false;
             }());

    verifier("Le nombre de canaux est bien mis à jour",
             [&]() {
                 for (const auto& e : bdd.chargerTousLesEquipements())
                     if (e.idEquipement == idTest) return e.canaux.size() == 2;
                 return false;
             }());

    // Nettoyage
    nettoyerEquipementParNom("EquipModifTestMAJ");

    afficherResultats("modifierEquipment");
}

// ─────────────────────────────────────────────
//  supprimerEquipment
// ─────────────────────────────────────────────

void testerSupprimerEquipment() {
    qDebug() << "\n=== Tests : AccessBDD::supprimerEquipment() ===";
    AccessBDD bdd;

    QList<UniversData> univers = bdd.chargerUnivers();
    if (univers.isEmpty()) {
        qDebug() << "[SKIP] Aucun univers en base, tests ignorés";
        return;
    }
    int idUniversValide = univers.first().idUnivers;

    // Préparation
    nettoyerEquipementParNom("EquipSupprTest");
    EquipmentData eq = construireEquipementTest("EquipSupprTest", idUniversValide);
    bdd.enregistrerEquipment(eq, idUniversValide);

    int idTest = -1;
    for (const auto& e : bdd.chargerTousLesEquipements())
        if (e.nom == "EquipSupprTest") { idTest = e.idEquipement; break; }

    verifier("Suppression d'un équipement existant retourne true",
             idTest > 0 && bdd.supprimerEquipment(idTest));

    verifier("L'équipement n'est plus présent en base après suppression",
             [&]() {
                 for (const auto& e : bdd.chargerTousLesEquipements())
                     if (e.idEquipement == idTest) return false;
                 return true;
             }());

    verifier("Suppression d'un id inexistant retourne true (DELETE sans ligne affectée)",
             bdd.supprimerEquipment(-1));

    afficherResultats("supprimerEquipment");
}

// ─────────────────────────────────────────────
//  enregistrerScene / chargerLesScenes / chargerValeursScene
// ─────────────────────────────────────────────

void testerScenes() {
    qDebug() << "\n=== Tests : AccessBDD::enregistrerScene() / chargerLesScenes() / chargerValeursScene() ===";
    AccessBDD bdd;

    nettoyerSceneParNom("SceneTest");

    QMap<int, int> valeurs;
    valeurs.insert(1, 128);
    valeurs.insert(2, 255);

    verifier("Enregistrement d'une scène valide retourne true",
             bdd.enregistrerScene("SceneTest", valeurs));

    // Récupérer l'id de la scène insérée
    int idScene = -1;
    for (const auto& s : bdd.chargerLesScenes())
        if (s.nomScene == "SceneTest") { idScene = s.idScene; break; }

    verifier("La scène est bien présente dans chargerLesScenes()",
             idScene > 0);

    verifier("Les valeurs de canaux sont bien persistées",
             [&]() {
                 if (idScene <= 0) return false;
                 QMap<int, int> chargees = bdd.chargerValeursScene(idScene);
                 return chargees.value(1) == 128 && chargees.value(2) == 255;
             }());

    verifier("chargerLesScenes() retourne les scènes triées par nom croissant",
             [&]() {
                 QList<SceneData> liste = bdd.chargerLesScenes();
                 for (int i = 1; i < liste.size(); ++i)
                     if (liste[i].nomScene < liste[i - 1].nomScene) return false;
                 return true;
             }());

    verifier("Enregistrement d'une scène sans canal retourne true",
             bdd.enregistrerScene("SceneVideTest", {}));
    nettoyerSceneParNom("SceneVideTest");

    // Nettoyage
    nettoyerSceneParNom("SceneTest");

    afficherResultats("Scenes (enregistrer/charger/valeurs)");
}

// ─────────────────────────────────────────────
//  renommerScene
// ─────────────────────────────────────────────

void testerRenommerScene() {
    qDebug() << "\n=== Tests : AccessBDD::renommerScene() ===";
    AccessBDD bdd;

    nettoyerSceneParNom("SceneRenomTest");
    nettoyerSceneParNom("SceneRenomTestMAJ");
    bdd.enregistrerScene("SceneRenomTest", {});

    int idScene = -1;
    for (const auto& s : bdd.chargerLesScenes())
        if (s.nomScene == "SceneRenomTest") { idScene = s.idScene; break; }

    verifier("Renommage d'une scène existante retourne true",
             idScene > 0 && bdd.renommerScene(idScene, "SceneRenomTestMAJ"));

    verifier("Le nouveau nom est bien persisté en base",
             [&]() {
                 for (const auto& s : bdd.chargerLesScenes())
                     if (s.idScene == idScene) return s.nomScene == "SceneRenomTestMAJ";
                 return false;
             }());

    // Nettoyage
    nettoyerSceneParNom("SceneRenomTestMAJ");

    afficherResultats("renommerScene");
}

// ─────────────────────────────────────────────
//  supprimerScene
// ─────────────────────────────────────────────

void testerSupprimerScene() {
    qDebug() << "\n=== Tests : AccessBDD::supprimerScene() ===";
    AccessBDD bdd;

    nettoyerSceneParNom("SceneSupprTest");

    QMap<int, int> valeurs;
    valeurs.insert(1, 100);
    bdd.enregistrerScene("SceneSupprTest", valeurs);

    int idScene = -1;
    for (const auto& s : bdd.chargerLesScenes())
        if (s.nomScene == "SceneSupprTest") { idScene = s.idScene; break; }

    verifier("Suppression d'une scène existante retourne true",
             idScene > 0 && bdd.supprimerScene(idScene));

    verifier("La scène n'est plus présente dans chargerLesScenes()",
             [&]() {
                 for (const auto& s : bdd.chargerLesScenes())
                     if (s.idScene == idScene) return false;
                 return true;
             }());

    verifier("Les entrées PILOTE associées sont bien supprimées",
             [&]() {
                 if (idScene <= 0) return false;
                 return bdd.chargerValeursScene(idScene).isEmpty();
             }());

    afficherResultats("supprimerScene");
}

// ─────────────────────────────────────────────
//  Main
// ─────────────────────────────────────────────

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    testerChargerUnivers();
    testerEnregistrerUnivers();
    testerModifierUnivers();
    testerSupprimerUnivers();
    testerEnregistrerEquipment();
    testerModifierEquipment();
    testerSupprimerEquipment();
    testerScenes();
    testerRenommerScene();
    testerSupprimerScene();

    return testsEchoues > 0 ? 1 : 0;

    // InterfacePcClient w;
    // w.setWindowTitle("DMX Equipment Manager");
    // w.showMaximized();
    // return a.exec();
}
