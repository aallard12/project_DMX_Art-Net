/**
 * @file accessbdd.h
 * @brief Déclaration de la classe AccessBDD
 * @author Antoine ALLARD
 * @date 10/04/2026
 */

#ifndef ACCESSBDD_H
#define ACCESSBDD_H

#include <QSqlDatabase>
#include <QFileInfo>
#include <QSettings>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QList>

/**
 * @struct UniversData
 * @brief Regroupe les informations d'identification d'un univers DMX.
 */
struct UniversData {
    int idUnivers;
    int numero;
    QString ip;
};

/**
 * @struct FunctionData
 * @brief Définit une plage de valeurs DMX pour une fonction précise d'un canal.
 */
struct FunctionData {
    QString nom;
    QString min;
    QString max;
};

/**
 * @struct ChannelData
 * @brief Représente un canal DMX individuel au sein d'un équipement.
 */
struct ChannelData {
    QString description;
    QList<FunctionData> fonctions;
};

/**
 * @struct EquipmentData
 * @brief Modélise un appareil DMX complet (projecteur, lyre, etc.).
 */
struct EquipmentData {
    int idEquipement;
    QString nom;
    QString univers;
    QString dmxStart;
    QList<ChannelData> canaux;
    QString couleur;
};

/**
 * @struct DmxFunctionInfo
 * @brief Version simplifiée d'une fonction pour le traitement des données Live.
 */
struct DmxFunctionInfo {
    int min;
    int max;
    QString nom;
};

/**
 * @struct DmxChannelInfo
 * @brief Structure détaillée d'un canal pour la cartographie d'un univers.
 */
struct DmxChannelInfo {
    int idCanal = -1;
    QString nomEquipement;
    QString couleur;
    QString description;
    QList<DmxFunctionInfo> fonctions;
};

/**
 * @struct SceneData
 * @brief Représente une scène (un état lumineux) enregistrée.
 */
struct SceneData {
    int idScene;
    QString nomScene;
};

class AccessBDD {
private:
    /// Objet gérant la connexion et les requêtes vers la base de données SQL
    QSqlDatabase bdd;
    void initialiserSchema();
public:
    AccessBDD();
    QList<UniversData> chargerUnivers();

    bool enregistrerUnivers(int numero, const QString& ip);
    bool modifierUnivers(int idUnivers, int numero, const QString& ip);
    bool supprimerUnivers(int idUnivers);

    bool enregistrerEquipment(const EquipmentData& eq, int idUniversSelectionne);
    bool supprimerEquipment(int idEquipement);
    bool modifierEquipment(int idEquipement, const EquipmentData& eq, int idUniversSelectionne);

    QList<EquipmentData> chargerTousLesEquipements();

    QMap<int, DmxChannelInfo> chargerMapUnivers(int idUnivers);

    bool enregistrerScene(const QString& nomScene, const QMap<int, int>& valeursCanaux);

    int recupererCompteurCanaux(int index);
    QList<SceneData> chargerLesScenes();
    QMap<int, int> chargerValeursScene(int idScene);

    int getUniversDeScene(int idScene);
    bool renommerScene(int idScene, const QString& nouveauNom);
    bool supprimerScene(int idScene);
};

#endif
