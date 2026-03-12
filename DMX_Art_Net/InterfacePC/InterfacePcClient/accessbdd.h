#ifndef ACCESSBDD_H
#define ACCESSBDD_H

#include <QSqlDatabase>
#include <QFileInfo>
#include <QSettings>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QList>
#include <QString>

struct UniversData {
    int idUnivers; // Clef primaire
    int numero;     // numero_Univers
    QString ip;     // adresse_Ip
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
};


class AccessBDD
{
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
    QList<EquipmentData> chargerTousLesEquipements(); // Pour synchroniser l'affichage

    void chargerUneScène();
    void chargerLesScènes();
};

#endif // ACCESSBDD_H
