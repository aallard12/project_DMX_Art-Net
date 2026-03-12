#include "accessbdd.h"


AccessBDD::AccessBDD()
{
    // QSqlDatabase bdd;
    // bdd = QSqlDatabase::addDatabase("QMYSQL");

    // QString ip,base,log,mdp;
    // QString nomFichierIni="commandes.ini";
    // QFileInfo testFichier(nomFichierIni);

    // if (testFichier.exists() && testFichier.isFile())
    // {
    //     QSettings paramsSocket(nomFichierIni,QSettings::IniFormat);

    //     ip = paramsSocket.value("CONFIG/hostname", "192.168.1.20").toString();
    //     base = paramsSocket.value("CONFIG/BDD", "DMXBDD").toString();
    //     log = paramsSocket.value("CONFIG/username", "adminDMX").toString();
    //     mdp = paramsSocket.value("CONFIG/password", "toto").toString();

    //     qDebug()<<ip<<base<<log<<mdp;
    // } else {
    //     qDebug()<<"fichier ini non valide ou non present";
    // }

    // // ip =  "192.168.1.20";
    // // base = "DMXBDD";
    // // log = "adminDMX";
    // // mdp = "toto";

    // bdd.setHostName(ip);
    // bdd.setDatabaseName(base);
    // bdd.setUserName(log);
    // bdd.setPassword(mdp);

    // bool ok = bdd.open();
    // if (!ok)
    // {
    //     QMessageBox::warning(nullptr,"Erreur de connexion à la bdd ",bdd.lastError().text());
    // }
    // else
    // {
    //     qDebug()<<"Ouverture de la BDD ok";
    // }
//----------------------------------------------------------------------------------------------------------

    QSqlDatabase bdd;
    bdd = QSqlDatabase::addDatabase("QMYSQL");

    QString ip,base,log,mdp;
    QString nomFichierIni="commandes.ini";
    QFileInfo testFichier(nomFichierIni);

    // if (testFichier.exists() && testFichier.isFile())
    // {
    //     QSettings paramsSocket(nomFichierIni,QSettings::IniFormat);

    //     ip = paramsSocket.value("CONFIG/hostname", "172.18.58.8").toString();
    //     base = paramsSocket.value("CONFIG/BDD", "DMXBDD").toString();
    //     log = paramsSocket.value("CONFIG/username", "ciel").toString();
    //     mdp = paramsSocket.value("CONFIG/password", "ciel").toString();

    //     qDebug()<<ip<<base<<log<<mdp;
    // } else {
    //     qDebug()<<"fichier ini non valide ou non present";
    // }

    ip =  "172.18.58.8";
    base = "DMXBDD";
    log = "ciel";
    mdp = "ciel";

    bdd.setHostName(ip);
    bdd.setDatabaseName(base);
    bdd.setUserName(log);
    bdd.setPassword(mdp);

    bool ok = bdd.open();
    if (!ok)
    {
        QMessageBox::warning(nullptr,"Erreur de connexion à la bdd ",bdd.lastError().text());
    }
    else
    {

        qDebug()<<ip<<base<<log<<mdp;
        qDebug()<<"Ouverture de la BDD ok";
    }
}

void AccessBDD::chargerEquipment()
{

}

void AccessBDD::chargerUnivers()
{

}

void AccessBDD::chargerUneScène()
{

}

void AccessBDD::chargerLesScènes()
{

}

void AccessBDD::enregistrerEquipment()
{

}
