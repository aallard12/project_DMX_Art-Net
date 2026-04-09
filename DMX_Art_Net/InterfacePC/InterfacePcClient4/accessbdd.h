#ifndef ACCESSBDD_H
#define ACCESSBDD_H

#include <QSqlDatabase>
#include <QFileInfo>
#include <QSettings>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QList>

struct UniversData {
    int idUnivers;
    int numero;
    QString ip;
};

struct FunctionData {
    QString nom;
    QString min;
    QString max;
};

struct ChannelData {
    QString description;
    QList<FunctionData> fonctions;
};

struct EquipmentData {
    int idEquipement;
    QString nom;
    QString univers;
    QString dmxStart;
    QList<ChannelData> canaux;
    QString couleur;
};

struct DmxFunctionInfo {
    int min;
    int max;
    QString nom;
};

struct DmxChannelInfo {
    int idCanal = -1;
    QString nomEquipement;
    QString couleur;
    QString description;
    QList<DmxFunctionInfo> fonctions;
};

struct SceneData {
    int idScene;
    QString nomScene;
};

class AccessBDD {
private:
    QSqlDatabase bdd;
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
