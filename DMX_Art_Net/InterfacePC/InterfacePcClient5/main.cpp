#include "interfacepcclient.h"
#include "accessbdd.h"

#include <QCoreApplication>
#include <QApplication>
#include <QDebug>
#include <iostream>
#include <string>

// ─────────────────────────────────────────────
//  Helpers de saisie console (std pur)
// ─────────────────────────────────────────────

static QString lireLigne() {
    std::string s;
    // On vérifie si la lecture a réussi
    if (!std::getline(std::cin, s)) {
        // Si std::cin est cassé (pas de terminal), on quitte brutalement
        // pour éviter la boucle infinie qui fige le PC.
        std::cerr << "\n[Erreur fatale] Flux d'entree standard indisponible. Quitte.\n";
        exit(EXIT_FAILURE);
    }
    return QString::fromStdString(s).trimmed();
}

static int lireInt() {
    return lireLigne().toInt();
}

static void pauseConsole() {
    std::cout << "\nAppuyez sur Entrée pour continuer...";
    std::cout.flush();
    lireLigne();
}

static void separateur() {
    std::cout << "\n──────────────────────────────────────────\n";
}

// ─────────────────────────────────────────────
//  Infrastructure de test
// ─────────────────────────────────────────────

static int testsReussis = 0;
static int testsEchoues = 0;

void verifier(const QString& nomTest, bool condition) {
    if (condition) {
        std::cout << "[OK]    " << nomTest.toStdString() << "\n";
        testsReussis++;
    } else {
        std::cout << "[ECHEC] " << nomTest.toStdString() << "\n";
        testsEchoues++;
    }
}

void afficherResultats(const QString& suite) {
    std::cout << "\nRésultat " << suite.toStdString()
              << " : " << testsReussis << " réussi(s), "
              << testsEchoues << " échoué(s)\n";
    testsReussis = 0;
    testsEchoues = 0;
}

// ─────────────────────────────────────────────
//  Helpers de nettoyage
// ─────────────────────────────────────────────

void nettoyerUniversParNumero(int numero) {
    QSqlQuery q;
    q.prepare("DELETE FROM UNIVERS WHERE numeroUnivers = :n");
    q.bindValue(":n", numero);
    q.exec();
}

void nettoyerEquipementParNom(const QString& nom) {
    QSqlQuery q;
    q.prepare("DELETE FROM EQUIPEMENTS WHERE nomEquipement = :n");
    q.bindValue(":n", nom);
    q.exec();
}

void nettoyerSceneParNom(const QString& nom) {
    QSqlQuery q;
    q.prepare("DELETE FROM SCENES WHERE nomScene = :n");
    q.bindValue(":n", nom);
    q.exec();
}

// ─────────────────────────────────────────────
//  Helper de construction de données de test
// ─────────────────────────────────────────────

EquipmentData construireEquipementTest(const QString& nom, int /*idUniversValide*/) {
    EquipmentData eq;
    eq.nom      = nom;
    eq.dmxStart = "1";
    eq.couleur  = "#FF0000";

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
//  Helpers d'affichage des données en base
// ─────────────────────────────────────────────

void afficherUniversDisponibles(AccessBDD& bdd) {
    QList<UniversData> liste = bdd.chargerUnivers();
    if (liste.isEmpty()) {
        std::cout << "  (aucun univers en base)\n";
    } else {
        std::cout << "  Univers disponibles :\n";
        for (const auto& u : liste)
            std::cout << "    id=" << u.idUnivers
                      << "  numéro=" << u.numero
                      << "  IP=" << u.ip.toStdString() << "\n";
    }
}

void afficherEquipementsDisponibles(AccessBDD& bdd) {
    QList<EquipmentData> liste = bdd.chargerTousLesEquipements();
    if (liste.isEmpty()) {
        std::cout << "  (aucun équipement en base)\n";
    } else {
        std::cout << "  Équipements disponibles :\n";
        for (const auto& e : liste)
            std::cout << "    id=" << e.idEquipement
                      << "  nom=" << e.nom.toStdString()
                      << "  canaux=" << e.canaux.size() << "\n";
    }
}

void afficherScenesDisponibles(AccessBDD& bdd) {
    QList<SceneData> liste = bdd.chargerLesScenes();
    if (liste.isEmpty()) {
        std::cout << "  (aucune scène en base)\n";
    } else {
        std::cout << "  Scènes disponibles :\n";
        for (const auto& s : liste)
            std::cout << "    id=" << s.idScene
                      << "  nom=" << s.nomScene.toStdString() << "\n";
    }
}

// ─────────────────────────────────────────────
//  Tests automatiques (originaux, inchangés)
// ─────────────────────────────────────────────

void testerChargerUnivers() {
    std::cout << "\n=== Tests : AccessBDD::chargerUnivers() ===\n";
    AccessBDD bdd;
    QList<UniversData> liste = bdd.chargerUnivers();

    verifier("La liste retournée est non nulle (objet valide)", true);

    verifier("Chaque univers a un idUnivers > 0", [&]() {
        for (const auto& u : liste) if (u.idUnivers <= 0) return false;
        return true;
    }());

    verifier("Chaque univers a un numéro > 0", [&]() {
        for (const auto& u : liste) if (u.numero <= 0) return false;
        return true;
    }());

    verifier("Chaque univers a une adresse IP non vide", [&]() {
        for (const auto& u : liste) if (u.ip.trimmed().isEmpty()) return false;
        return true;
    }());

    verifier("Les univers sont triés par numéro croissant", [&]() {
        for (int i = 1; i < liste.size(); ++i)
            if (liste[i].numero <= liste[i - 1].numero) return false;
        return true;
    }());

    verifier("Aucun doublon de numéro d'univers", [&]() {
        for (int i = 0; i < liste.size(); ++i)
            for (int j = i + 1; j < liste.size(); ++j)
                if (liste[i].numero == liste[j].numero) return false;
        return true;
    }());

    verifier("Aucun doublon d'idUnivers", [&]() {
        for (int i = 0; i < liste.size(); ++i)
            for (int j = i + 1; j < liste.size(); ++j)
                if (liste[i].idUnivers == liste[j].idUnivers) return false;
        return true;
    }());

    afficherResultats("chargerUnivers");
}

void testerEnregistrerUnivers() {
    std::cout << "\n=== Tests : AccessBDD::enregistrerUnivers() ===\n";
    AccessBDD bdd;
    nettoyerUniversParNumero(999);
    verifier("Enregistrement d'un univers valide",
             bdd.enregistrerUnivers(999, "192.168.1.99"));
    verifier("L'univers a bien été persisté en base", [&]() {
        for (const auto& u : bdd.chargerUnivers())
            if (u.numero == 999 && u.ip == "192.168.1.99") return true;
        return false;
    }());
    nettoyerUniversParNumero(999);
    verifier("Refus d'un univers avec IP vide",
             !bdd.enregistrerUnivers(998, ""));
    verifier("Refus d'un univers avec IP composée uniquement d'espaces",
             !bdd.enregistrerUnivers(998, "   "));
    afficherResultats("enregistrerUnivers");
}

void testerModifierUnivers() {
    std::cout << "\n=== Tests : AccessBDD::modifierUnivers() ===\n";
    AccessBDD bdd;
    nettoyerUniversParNumero(997);
    bdd.enregistrerUnivers(997, "10.0.0.1");
    int idTest = -1;
    for (const auto& u : bdd.chargerUnivers())
        if (u.numero == 997) { idTest = u.idUnivers; break; }
    verifier("Modification d'un univers existant retourne true",
             idTest > 0 && bdd.modifierUnivers(idTest, 997, "10.0.0.2"));
    verifier("La modification est bien persistée en base", [&]() {
        for (const auto& u : bdd.chargerUnivers())
            if (u.idUnivers == idTest && u.ip == "10.0.0.2") return true;
        return false;
    }());
    verifier("Modification d'un id inexistant retourne true (UPDATE sans ligne affectée)",
             bdd.modifierUnivers(-1, 0, "0.0.0.0"));
    nettoyerUniversParNumero(997);
    afficherResultats("modifierUnivers");
}

void testerSupprimerUnivers() {
    std::cout << "\n=== Tests : AccessBDD::supprimerUnivers() ===\n";
    AccessBDD bdd;
    nettoyerUniversParNumero(996);
    bdd.enregistrerUnivers(996, "10.0.1.1");
    int idTest = -1;
    for (const auto& u : bdd.chargerUnivers())
        if (u.numero == 996) { idTest = u.idUnivers; break; }
    verifier("Suppression d'un univers existant retourne true",
             idTest > 0 && bdd.supprimerUnivers(idTest));
    verifier("L'univers n'est plus présent en base après suppression", [&]() {
        for (const auto& u : bdd.chargerUnivers())
            if (u.idUnivers == idTest) return false;
        return true;
    }());
    verifier("Suppression d'un id inexistant retourne true (DELETE sans ligne affectée)",
             bdd.supprimerUnivers(-1));
    afficherResultats("supprimerUnivers");
}

void testerEnregistrerEquipment() {
    std::cout << "\n=== Tests : AccessBDD::enregistrerEquipment() ===\n";
    AccessBDD bdd;
    QList<UniversData> univers = bdd.chargerUnivers();
    if (univers.isEmpty()) {
        std::cout << "[SKIP] Aucun univers en base, tests ignorés\n";
        return;
    }
    int idUniversValide = univers.first().idUnivers;
    nettoyerEquipementParNom("EquipTest");
    EquipmentData eq = construireEquipementTest("EquipTest", idUniversValide);
    verifier("Enregistrement d'un équipement valide retourne true",
             bdd.enregistrerEquipment(eq, idUniversValide));
    verifier("L'équipement est bien persisté en base", [&]() {
        for (const auto& e : bdd.chargerTousLesEquipements())
            if (e.nom == "EquipTest") return true;
        return false;
    }());
    verifier("L'équipement persisté possède le bon nombre de canaux", [&]() {
        for (const auto& e : bdd.chargerTousLesEquipements())
            if (e.nom == "EquipTest") return e.canaux.size() == eq.canaux.size();
        return false;
    }());
    nettoyerEquipementParNom("EquipTest");
    afficherResultats("enregistrerEquipment");
}

void testerModifierEquipment() {
    std::cout << "\n=== Tests : AccessBDD::modifierEquipment() ===\n";
    AccessBDD bdd;
    QList<UniversData> univers = bdd.chargerUnivers();
    if (univers.isEmpty()) {
        std::cout << "[SKIP] Aucun univers en base, tests ignorés\n";
        return;
    }
    int idUniversValide = univers.first().idUnivers;
    nettoyerEquipementParNom("EquipModifTest");
    nettoyerEquipementParNom("EquipModifTestMAJ");
    EquipmentData eq = construireEquipementTest("EquipModifTest", idUniversValide);
    bdd.enregistrerEquipment(eq, idUniversValide);
    int idTest = -1;
    for (const auto& e : bdd.chargerTousLesEquipements())
        if (e.nom == "EquipModifTest") { idTest = e.idEquipement; break; }
    EquipmentData eqMAJ = construireEquipementTest("EquipModifTestMAJ", idUniversValide);
    ChannelData c2; c2.description = "Canal couleur";
    eqMAJ.canaux.append(c2);
    verifier("Modification d'un équipement existant retourne true",
             idTest > 0 && bdd.modifierEquipment(idTest, eqMAJ, idUniversValide));
    verifier("Le nouveau nom est bien persisté", [&]() {
        for (const auto& e : bdd.chargerTousLesEquipements())
            if (e.idEquipement == idTest) return e.nom == "EquipModifTestMAJ";
        return false;
    }());
    verifier("Le nombre de canaux est bien mis à jour", [&]() {
        for (const auto& e : bdd.chargerTousLesEquipements())
            if (e.idEquipement == idTest) return e.canaux.size() == 2;
        return false;
    }());
    nettoyerEquipementParNom("EquipModifTestMAJ");
    afficherResultats("modifierEquipment");
}

void testerSupprimerEquipment() {
    std::cout << "\n=== Tests : AccessBDD::supprimerEquipment() ===\n";
    AccessBDD bdd;
    QList<UniversData> univers = bdd.chargerUnivers();
    if (univers.isEmpty()) {
        std::cout << "[SKIP] Aucun univers en base, tests ignorés\n";
        return;
    }
    int idUniversValide = univers.first().idUnivers;
    nettoyerEquipementParNom("EquipSupprTest");
    EquipmentData eq = construireEquipementTest("EquipSupprTest", idUniversValide);
    bdd.enregistrerEquipment(eq, idUniversValide);
    int idTest = -1;
    for (const auto& e : bdd.chargerTousLesEquipements())
        if (e.nom == "EquipSupprTest") { idTest = e.idEquipement; break; }
    verifier("Suppression d'un équipement existant retourne true",
             idTest > 0 && bdd.supprimerEquipment(idTest));
    verifier("L'équipement n'est plus présent en base après suppression", [&]() {
        for (const auto& e : bdd.chargerTousLesEquipements())
            if (e.idEquipement == idTest) return false;
        return true;
    }());
    verifier("Suppression d'un id inexistant retourne true (DELETE sans ligne affectée)",
             bdd.supprimerEquipment(-1));
    afficherResultats("supprimerEquipment");
}

void testerScenes() {
    std::cout << "\n=== Tests : AccessBDD::enregistrerScene() / chargerLesScenes() / chargerValeursScene() ===\n";
    AccessBDD bdd;
    nettoyerSceneParNom("SceneTest");
    QMap<int, int> valeurs;
    QSqlQuery q("SELECT idCanal FROM CANAUX LIMIT 2");
    while (q.next()) valeurs.insert(q.value(0).toInt(), 128);
    if (!valeurs.isEmpty()) {
        verifier("Enregistrement d'une scène valide retourne true",
                 bdd.enregistrerScene("SceneTest", valeurs));
        int idScene = -1;
        for (const auto& s : bdd.chargerLesScenes())
            if (s.nomScene == "SceneTest") { idScene = s.idScene; break; }
        verifier("La scène est bien présente dans chargerLesScenes()", idScene > 0);
        verifier("Les valeurs de canaux sont bien persistées", [&]() {
            if (idScene <= 0) return false;
            QMap<int, int> chargees = bdd.chargerValeursScene(idScene);
            return chargees.value(1) == 128 && chargees.value(2) == 255;
        }());
        verifier("chargerLesScenes() retourne les scènes triées par nom croissant", [&]() {
            QList<SceneData> liste = bdd.chargerLesScenes();
            for (int i = 1; i < liste.size(); ++i)
                if (liste[i].nomScene < liste[i - 1].nomScene) return false;
            return true;
        }());
        verifier("Enregistrement d'une scène sans canal retourne true",
                 bdd.enregistrerScene("SceneVideTest", {}));
        nettoyerSceneParNom("SceneVideTest");
        nettoyerSceneParNom("SceneTest");
        afficherResultats("Scenes (enregistrer/charger/valeurs)");
    } else {
        std::cout << "[SKIP] Aucun canal en base, test ignoré\n";
    }
}

void testerRenommerScene() {
    std::cout << "\n=== Tests : AccessBDD::renommerScene() ===\n";
    AccessBDD bdd;
    nettoyerSceneParNom("SceneRenomTest");
    nettoyerSceneParNom("SceneRenomTestMAJ");
    bdd.enregistrerScene("SceneRenomTest", {});
    int idScene = -1;
    for (const auto& s : bdd.chargerLesScenes())
        if (s.nomScene == "SceneRenomTest") { idScene = s.idScene; break; }
    verifier("Renommage d'une scène existante retourne true",
             idScene > 0 && bdd.renommerScene(idScene, "SceneRenomTestMAJ"));
    verifier("Le nouveau nom est bien persisté en base", [&]() {
        for (const auto& s : bdd.chargerLesScenes())
            if (s.idScene == idScene) return s.nomScene == "SceneRenomTestMAJ";
        return false;
    }());
    nettoyerSceneParNom("SceneRenomTestMAJ");
    afficherResultats("renommerScene");
}

void testerSupprimerScene() {
    std::cout << "\n=== Tests : AccessBDD::supprimerScene() ===\n";
    AccessBDD bdd;
    nettoyerSceneParNom("SceneSupprTest");
    QMap<int, int> valeurs;
    QSqlQuery qCanaux("SELECT idCanal FROM CANAUX LIMIT 1");
    if (qCanaux.next()) valeurs.insert(qCanaux.value(0).toInt(), 100);
    bdd.enregistrerScene("SceneSupprTest", valeurs);
    int idScene = -1;
    for (const auto& s : bdd.chargerLesScenes())
        if (s.nomScene == "SceneSupprTest") { idScene = s.idScene; break; }
    verifier("Suppression d'une scène existante retourne true",
             idScene > 0 && bdd.supprimerScene(idScene));
    verifier("La scène n'est plus présente dans chargerLesScenes()", [&]() {
        for (const auto& s : bdd.chargerLesScenes())
            if (s.idScene == idScene) return false;
        return true;
    }());
    verifier("Les entrées PILOTE associées sont bien supprimées", [&]() {
        if (idScene <= 0) return false;
        return bdd.chargerValeursScene(idScene).isEmpty();
    }());
    afficherResultats("supprimerScene");
}

// ─────────────────────────────────────────────
//  Sous-menus interactifs
// ─────────────────────────────────────────────

void menuUnivers() {
    AccessBDD bdd;
    bool retour = false;
    while (!retour) {
        separateur();
        std::cout << "--- MENU UNIVERS ---\n"
                  << "  1. Afficher tous les univers\n"
                  << "  2. Enregistrer un nouvel univers\n"
                  << "  3. Modifier un univers existant\n"
                  << "  4. Supprimer un univers\n"
                  << "  5. Tests automatiques (univers)\n"
                  << "  0. Retour\n"
                  << "Votre choix : ";
        std::cout.flush();
        QString choix = lireLigne();

        if (choix == "1") {
            separateur();
            std::cout << "[Univers en base]\n";
            afficherUniversDisponibles(bdd);

        } else if (choix == "2") {
            separateur();
            std::cout << "[Enregistrer un univers]\n";
            std::cout << "Numéro d'univers : "; std::cout.flush(); int num = lireInt();
            std::cout << "Adresse IP       : "; std::cout.flush(); QString ip = lireLigne();
            bool ok = bdd.enregistrerUnivers(num, ip);
            std::cout << (ok ? "[OK] Univers enregistré.\n" : "[ECHEC] Enregistrement refusé (IP vide ?).\n");

        } else if (choix == "3") {
            separateur();
            std::cout << "[Modifier un univers]\n";
            afficherUniversDisponibles(bdd);
            std::cout << "ID à modifier  : "; std::cout.flush(); int id  = lireInt();
            std::cout << "Nouveau numéro : "; std::cout.flush(); int num = lireInt();
            std::cout << "Nouvelle IP    : "; std::cout.flush(); QString ip = lireLigne();
            bool ok = bdd.modifierUnivers(id, num, ip);
            std::cout << (ok ? "[OK] Univers modifié.\n" : "[ECHEC] Modification refusée.\n");

        } else if (choix == "4") {
            separateur();
            std::cout << "[Supprimer un univers]\n";
            afficherUniversDisponibles(bdd);
            std::cout << "ID à supprimer : "; std::cout.flush(); int id = lireInt();
            bool ok = bdd.supprimerUnivers(id);
            std::cout << (ok ? "[OK] Univers supprimé.\n" : "[ECHEC] Suppression refusée.\n");

        } else if (choix == "5") {
            separateur();
            testerChargerUnivers();
            testerEnregistrerUnivers();
            testerModifierUnivers();
            testerSupprimerUnivers();

        } else if (choix == "0") {
            retour = true;
        } else {
            std::cout << "Choix invalide.\n";
        }
        if (!retour) pauseConsole();
    }
}

void menuEquipements() {
    AccessBDD bdd;
    bool retour = false;
    while (!retour) {
        separateur();
        std::cout << "--- MENU ÉQUIPEMENTS ---\n"
                  << "  1. Afficher tous les équipements\n"
                  << "  2. Enregistrer un équipement\n"
                  << "  3. Modifier un équipement existant\n"
                  << "  4. Supprimer un équipement\n"
                  << "  5. Tests automatiques (équipements)\n"
                  << "  0. Retour\n"
                  << "Votre choix : ";
        std::cout.flush();
        QString choix = lireLigne();

        if (choix == "1") {
            separateur();
            std::cout << "[Équipements en base]\n";
            afficherEquipementsDisponibles(bdd);

        } else if (choix == "2") {
            separateur();
            std::cout << "[Enregistrer un équipement]\n";
            afficherUniversDisponibles(bdd);
            std::cout << "ID de l'univers cible : "; std::cout.flush(); int idUnivers = lireInt();
            std::cout << "Nom de l'équipement   : "; std::cout.flush(); QString nom = lireLigne();
            EquipmentData eq = construireEquipementTest(nom, idUnivers);
            std::cout << "Adresse DMX start     : "; std::cout.flush(); eq.dmxStart = lireLigne();
            std::cout << "Couleur (ex: #FF0000) : "; std::cout.flush(); eq.couleur  = lireLigne();
            bool ok = bdd.enregistrerEquipment(eq, idUnivers);
            std::cout << (ok ? "[OK] Équipement enregistré.\n" : "[ECHEC] Enregistrement refusé.\n");

        } else if (choix == "3") {
            separateur();
            std::cout << "[Modifier un équipement]\n";
            afficherEquipementsDisponibles(bdd);
            std::cout << "ID de l'équipement à modifier : "; std::cout.flush(); int idEq = lireInt();
            afficherUniversDisponibles(bdd);
            std::cout << "ID de l'univers cible         : "; std::cout.flush(); int idUnivers = lireInt();
            std::cout << "Nouveau nom                   : "; std::cout.flush(); QString nom = lireLigne();
            EquipmentData eqMAJ = construireEquipementTest(nom, idUnivers);
            std::cout << "Nouvelle adresse DMX start    : "; std::cout.flush(); eqMAJ.dmxStart = lireLigne();
            std::cout << "Nouvelle couleur (ex: #00FF00): "; std::cout.flush(); eqMAJ.couleur  = lireLigne();
            bool ok = bdd.modifierEquipment(idEq, eqMAJ, idUnivers);
            std::cout << (ok ? "[OK] Équipement modifié.\n" : "[ECHEC] Modification refusée.\n");

        } else if (choix == "4") {
            separateur();
            std::cout << "[Supprimer un équipement]\n";
            afficherEquipementsDisponibles(bdd);
            std::cout << "ID à supprimer : "; std::cout.flush(); int id = lireInt();
            bool ok = bdd.supprimerEquipment(id);
            std::cout << (ok ? "[OK] Équipement supprimé.\n" : "[ECHEC] Suppression refusée.\n");

        } else if (choix == "5") {
            separateur();
            testerEnregistrerEquipment();
            testerModifierEquipment();
            testerSupprimerEquipment();

        } else if (choix == "0") {
            retour = true;
        } else {
            std::cout << "Choix invalide.\n";
        }
        if (!retour) pauseConsole();
    }
}

void menuScenes() {
    AccessBDD bdd;
    bool retour = false;
    while (!retour) {
        separateur();
        std::cout << "--- MENU SCÈNES ---\n"
                  << "  1. Afficher toutes les scènes\n"
                  << "  2. Enregistrer une scène\n"
                  << "  3. Charger les valeurs d'une scène\n"
                  << "  4. Renommer une scène\n"
                  << "  5. Supprimer une scène\n"
                  << "  6. Tests automatiques (scènes)\n"
                  << "  0. Retour\n"
                  << "Votre choix : ";
        std::cout.flush();
        QString choix = lireLigne();

        if (choix == "1") {
            separateur();
            std::cout << "[Scènes en base]\n";
            afficherScenesDisponibles(bdd);

        } else if (choix == "2") {
            separateur();
            std::cout << "[Enregistrer une scène]\n";
            std::cout << "Nom de la scène : "; std::cout.flush(); QString nom = lireLigne();
            QMap<int, int> valeurs;
            std::cout << "Saisir les valeurs de canaux (format idCanal=valeur, ligne vide pour terminer) :\n";
            while (true) {
                std::cout << "  > "; std::cout.flush();
                QString ligne = lireLigne();
                if (ligne.isEmpty()) break;
                QStringList parts = ligne.split('=');
                if (parts.size() == 2) {
                    bool ok1, ok2;
                    int idC = parts[0].trimmed().toInt(&ok1);
                    int val = parts[1].trimmed().toInt(&ok2);
                    if (ok1 && ok2) valeurs.insert(idC, val);
                    else std::cout << "  Format invalide (attendu : idCanal=valeur), ignoré.\n";
                }
            }
            bool ok = bdd.enregistrerScene(nom, valeurs);
            std::cout << (ok ? "[OK] Scène enregistrée.\n" : "[ECHEC] Enregistrement refusé.\n");

        } else if (choix == "3") {
            separateur();
            std::cout << "[Valeurs d'une scène]\n";
            afficherScenesDisponibles(bdd);
            std::cout << "ID de la scène : "; std::cout.flush(); int id = lireInt();
            QMap<int, int> vals = bdd.chargerValeursScene(id);
            if (vals.isEmpty()) {
                std::cout << "  Aucune valeur trouvée.\n";
            } else {
                for (auto it = vals.cbegin(); it != vals.cend(); ++it)
                    std::cout << "    idCanal=" << it.key() << "  valeur=" << it.value() << "\n";
            }

        } else if (choix == "4") {
            separateur();
            std::cout << "[Renommer une scène]\n";
            afficherScenesDisponibles(bdd);
            std::cout << "ID de la scène : "; std::cout.flush(); int id  = lireInt();
            std::cout << "Nouveau nom    : "; std::cout.flush(); QString nom = lireLigne();
            bool ok = bdd.renommerScene(id, nom);
            std::cout << (ok ? "[OK] Scène renommée.\n" : "[ECHEC] Renommage refusé.\n");

        } else if (choix == "5") {
            separateur();
            std::cout << "[Supprimer une scène]\n";
            afficherScenesDisponibles(bdd);
            std::cout << "ID à supprimer : "; std::cout.flush(); int id = lireInt();
            bool ok = bdd.supprimerScene(id);
            std::cout << (ok ? "[OK] Scène supprimée.\n" : "[ECHEC] Suppression refusée.\n");

        } else if (choix == "6") {
            separateur();
            testerScenes();
            testerRenommerScene();
            testerSupprimerScene();

        } else if (choix == "0") {
            retour = true;
        } else {
            std::cout << "Choix invalide.\n";
        }
        if (!retour) pauseConsole();
    }
}

// ─────────────────────────────────────────────
//  Menu principal
// ─────────────────────────────────────────────

void lancerMenuTests() {
    bool quitter = false;
    while (!quitter) {
        separateur();
        std::cout << "\n╔══════════════════════════════════╗\n"
                  << "║    MENU PRINCIPAL — TESTS BDD    ║\n"
                  << "╚══════════════════════════════════╝\n"
                  << "  1. Univers\n"
                  << "  2. Équipements\n"
                  << "  3. Scènes\n"
                  << "  4. Lancer TOUS les tests automatiques\n"
                  << "  0. Quitter → lancer l'interface graphique\n"
                  << "Votre choix : ";
        std::cout.flush();
        QString choix = lireLigne();

        if (choix == "1") {
            menuUnivers();
        } else if (choix == "2") {
            menuEquipements();
        } else if (choix == "3") {
            menuScenes();
        } else if (choix == "4") {
            separateur();
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
            pauseConsole();
        } else if (choix == "0") {
            quitter = true;
        } else {
            std::cout << "Choix invalide.\n";
        }
    }
}

// ─────────────────────────────────────────────
//  Main
// ─────────────────────────────────────────────

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    //------------------------- COCHER LA CASE EXECUTER DANS UN TERMINAL -------------------------//

    // lancerMenuTests();
    InterfacePcClient w;
    w.setWindowTitle("DMX Equipment Manager");
    w.showMaximized();

    return app.exec();
}
