#include "interfacepcclient.h"
#include "testclassbdd.h"

#include <QCoreApplication>
#include <QTextStream>
#include <QStringList>
#include <iostream>
#include <QApplication>

namespace {

/** Affiche le menu principal et retourne le choix de l'utilisateur. */
int afficherMenu() {
    std::cout << "\n"
              << "╔══════════════════════════════════════════════════╗\n"
              << "║        Tests unitaires – AccessBDD (Qt Test)     ║\n"
              << "╠══════════════════════════════════════════════════╣\n"
              << "║  1. Configurer la connexion à la base de données ║\n"
              << "║  2. Lancer TOUS les tests                        ║\n"
              << "║  3. Lancer les tests – Univers                   ║\n"
              << "║  4. Lancer les tests – Équipements               ║\n"
              << "║  5. Lancer les tests – Scènes                    ║\n"
              << "║  6. Lancer les tests – Canaux / Map              ║\n"
              << "║  0. Quitter                                      ║\n"
              << "╚══════════════════════════════════════════════════╝\n"
              << "Votre choix : ";
    int choix = 0;
    std::cin >> choix;
    std::cin.ignore(1000, '\n');
    return choix;
}

/** Configure les paramètres de connexion via la console. */
void configurerConnexion(TestClassBDD& obj) {
    auto lire = [](const char* label, const QString& defaut) -> QString {
        std::cout << label << " [" << defaut.toStdString() << "] : ";
        std::string saisie;
        std::getline(std::cin, saisie);
        return saisie.empty() ? defaut : QString::fromStdString(saisie);
    };

    std::cout << "\n─── Paramètres de connexion ───\n"
              << "(Appuyez sur Entrée pour conserver la valeur par défaut)\n";
    obj.m_host     = lire("Hôte MySQL     ", obj.m_host);
    obj.m_base     = lire("Nom de la base ", obj.m_base);
    obj.m_user     = lire("Utilisateur    ", obj.m_user);
    obj.m_password = lire("Mot de passe   ", obj.m_password);
    std::cout << "✔ Connexion configurée.\n";
}

/**
 * Lance Qt Test en filtrant les tests selon le filtre donné.
 * Passe "-o -,txt" pour une sortie console colorée.
 */
int lancerTests(QCoreApplication& app, TestClassBDD& obj,
                const QStringList& filtres = {}) {

    QStringList argsList;
    argsList << app.arguments().first();
    for (const QString& f : filtres)   // ← un argument par test
        argsList << f;
    argsList << "-o" << "-,txt";

    // Convertir en argc/argv C
    int argc = argsList.size();
    std::vector<QByteArray> byteArrays;
    std::vector<char*> argv;
    for (const QString& s : argsList) {
        byteArrays.push_back(s.toLocal8Bit());
        argv.push_back(byteArrays.back().data());
    }
    return QTest::qExec(&obj, argc, argv.data());
}

} // namespace anonyme — fin des fonctions utilitaires

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    TestClassBDD testObj;
    int retCode = 0;

    // Si des arguments sont passés directement (ex. CI), on lance tout sans menu
    if (argc > 1) {
        return QTest::qExec(&testObj, argc, argv);
    }

    bool continuer = true;
    while (continuer) {
        int choix = afficherMenu();
        switch (choix) {
        case 0:
            continuer = false;
            break;
        case 1:
            configurerConnexion(testObj);
            break;
        case 2:
            retCode = lancerTests(app, testObj);
            break;
        case 3:
            // Filtre Qt Test : nom partiel du test (correspond à un sous-ensemble)
            retCode = lancerTests(app, testObj, {
                "test_enregistrerUnivers_valide",
                "test_enregistrerUnivers_ipVide",
                "test_enregistrerUnivers_numeroNegatif",
                "test_chargerUnivers_nonVide",
                "test_chargerUnivers_donneesCorrectes",
                "test_modifierUnivers_valide",
                "test_modifierUnivers_idInexistant",
                "test_supprimerUnivers_valide",
                "test_supprimerUnivers_idInexistant"
            });
            break;
        case 4:
            retCode = lancerTests(app, testObj, {
                            "test_enregistrerEquipment_valide",
                            "test_enregistrerEquipment_universInexistant",
                            "test_modifierEquipment_valide",
                            "test_modifierEquipment_idInexistant",
                            "test_supprimerEquipment_valide",
                            "test_supprimerEquipment_idInexistant",
                            "test_chargerTousLesEquipements_nonVide"
                        });
            break;
        case 5:
            retCode = lancerTests(app, testObj, {
                            "test_enregistrerScene_valide",
                            "test_enregistrerScene_mapVide",
                            "test_enregistrerScene_canalInexistant",
                            "test_chargerLesScenes_nonVide",
                            "test_chargerValeursScene_donneesCorrectes",
                            "test_chargerValeursScene_idInexistant",
                            "test_renommerScene_valide",
                            "test_renommerScene_idInexistant",
                            "test_supprimerScene_valide",
                            "test_supprimerScene_idInexistant",
                            "test_getUniversDeScene_valide",
                            "test_getUniversDeScene_idInexistant"
                        });
            break;
        case 6:
            retCode = lancerTests(app, testObj, {
                            "test_recupererCompteurCanaux_nonZero",
                            "test_recupererCompteurCanaux_zero",
                            "test_chargerMapUnivers_nonVide",
                            "test_chargerMapUnivers_universInexistant"
                        });
            break;
        default:
            std::cout << "Choix invalide.\n";
        }

        if (choix >= 2 && choix <= 6) {
            std::cout << "\n[Tests terminés – code retour : " << retCode << "]\n"
                      << "Appuyez sur Entrée pour revenir au menu…";
            std::cin.ignore(1000, '\n');
        }
    }
    return retCode;
}
