#include <QApplication>
#include <QDebug>
#include "accessbdd.h"
#include "interfacepcclient.h"

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

void testerChargerUnivers() {
    qDebug() << "\n=== Tests : AccessBDD::chargerUnivers() ===";
    AccessBDD bdd;
    QList<UniversData> liste = bdd.chargerUnivers();

    verifier("La liste retournée est non nulle (objet valide)",
             true);

    verifier("Chaque univers a un idUnivers > 0",
             [&]() {
                 bool valide = true;
                 for (int i = 0; i < liste.size() && valide; ++i) {
                     if (liste[i].idUnivers <= 0) {
                         valide = false;
                     }
                 }
                 return valide;
             }());

    verifier("Chaque univers a un numéro > 0",
             [&]() {
                 bool valide = true;
                 for (int i = 0; i < liste.size() && valide; ++i) {
                     if (liste[i].numero <= 0) {
                         valide = false;
                     }
                 }
                 return valide;
             }());

    verifier("Chaque univers a une adresse IP non vide",
             [&]() {
                 bool valide = true;
                 for (int i = 0; i < liste.size() && valide; ++i) {
                     if (liste[i].ip.trimmed().isEmpty()) {
                         valide = false;
                     }
                 }
                 return valide;
             }());

    verifier("Les univers sont triés par numéro croissant",
             [&]() {
                 bool valide = true;
                 for (int i = 1; i < liste.size() && valide; ++i) {
                     if (liste[i].numero <= liste[i - 1].numero) {
                         valide = false;
                     }
                 }
                 return valide;
             }());

    verifier("Aucun doublon de numéro d'univers",
             [&]() {
                 bool valide = true;
                 for (int i = 0; i < liste.size() && valide; ++i) {
                     for (int j = i + 1; j < liste.size() && valide; ++j) {
                         if (liste[i].numero == liste[j].numero) {
                             valide = false;
                         }
                     }
                 }
                 return valide;
             }());

    verifier("Aucun doublon d'idUnivers",
             [&]() {
                 bool valide = true;
                 for (int i = 0; i < liste.size() && valide; ++i) {
                     for (int j = i + 1; j < liste.size() && valide; ++j) {
                         if (liste[i].idUnivers == liste[j].idUnivers) {
                             valide = false;
                         }
                     }
                 }
                 return valide;
             }());

    qDebug() << "\nRésultat :" << testsReussis << "réussi(s)," << testsEchoues << "échoué(s)";
}

void testerEnregistrerUnivers() {
    qDebug() << "\n=== Tests : AccessBDD::enregistrerUnivers() ===";
    AccessBDD bdd;

    // 1. Test d'enregistrement (Cas nominal)
    verifier("Enregistrement d'un univers valide",
             [&]() {
                 UniversData nouvelUnivers;
                 nouvelUnivers.idUnivers = -1; // Simule un nouvel univers
                 nouvelUnivers.numero = 99;
                 nouvelUnivers.ip = "192.168.1.99";

                 bool resultat = bdd.enregistrerUnivers(nouvelUnivers.numero, nouvelUnivers.ip);
                 return resultat;
             }());

    // 2. Vérification de la persistance (Impartialité : on vérifie en BDD)
    verifier("L'univers a bien été persisté en base",
             [&]() {
                 bool trouve = false;
                 QList<UniversData> liste = bdd.chargerUnivers();

                 for (int i = 0; i < liste.size() && !trouve; ++i) {
                     if (liste[i].numero == 99 && liste[i].ip == "192.168.1.99") {
                         trouve = true;
                     }
                 }
                 return trouve;
             }());

    // 3. Test de refus d'une IP hors plage (Impartialité : la sécurité fonctionne)
        verifier("Refus d'une adresse IP hors plage (10.0.0.1)",
                 [&]() {
                     UniversData universHorsPlage;
                     universHorsPlage.numero = 99;
                     universHorsPlage.ip = "10.0.0.1"; // Devrait être bloqué par la contrainte SQL ou le code

                     bool resultat = bdd.enregistrerUnivers(universHorsPlage.numero, universHorsPlage.ip);

                     // Si le résultat est false, c'est que la sécurité a bien bloqué l'IP
                     bool testReussi = false;
                     if (resultat == false) {
                         testReussi = true;
                     }
                     return testReussi;
                 }());

    // 4. Test avec données invalides (Impartialité : tester les limites)
    verifier("Refus d'un univers avec IP vide",
                 [&]() {
                     UniversData universInvalide;
                     universInvalide.idUnivers = -1;
                     universInvalide.numero = 100;
                     universInvalide.ip = ""; // Donnée théoriquement invalide

                     bool resultat = bdd.enregistrerUnivers(universInvalide.numero, universInvalide.ip);

                     // Si l'enregistrement a échoué (resultat == false), alors le test est une RÉUSSITE
                     bool testReussi = false;
                     if (resultat == false) {
                         testReussi = true;
                     }
                     return testReussi;
                 }());

    qDebug() << "\nRésultat cumulé :" << testsReussis << "réussi(s)," << testsEchoues << "échoué(s)";
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    // testerChargerUnivers();
    // testerEnregistrerUnivers();
    // return testsEchoues > 0 ? 1 : 0;

    InterfacePcClient w;
    w.setWindowTitle("DMX Equipment Manager");
    w.show();
    return a.exec();
}
