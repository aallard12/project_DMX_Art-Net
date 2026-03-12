#ifndef ACCESSBDD_H
#define ACCESSBDD_H
#include <QSqlDatabase>
#include <QFileInfo>
#include <QSettings>
#include <QMessageBox>
#include <QSqlError>

class AccessBDD
{
private:
    QSqlDatabase bdd;
public:
    AccessBDD();
    void chargerEquipment();
    void chargerUnivers();
    void chargerUneScène();
    void chargerLesScènes();
    void enregistrerEquipment();
};

#endif // ACCESSBDD_H
