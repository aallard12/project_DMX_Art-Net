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
    if (!ok){
        QMessageBox::warning(nullptr,"Erreur de connexion à la bdd ",bdd.lastError().text());
    }else{
        qDebug()<<ip<<base<<log<<mdp;
        qDebug()<<"Ouverture de la BDD ok";
    }
}

QList<UniversData> AccessBDD::chargerUnivers()
{
    QList<UniversData> liste;
    QSqlQuery query("SELECT idUnivers, numeroUnivers, adresseIp FROM UNIVERS");

    while(query.next()) {
        UniversData u;
        u.idUnivers = query.value("idUnivers").toInt();
        u.numero = query.value("numeroUnivers").toInt();
        u.ip = query.value("adresseIp").toString();
        liste.append(u);
    }

    return liste;
}

bool AccessBDD::enregistrerUnivers(int numero, const QString &ip)
{
    bool succes = false;
    QSqlQuery query;
    // On utilise les noms exacts de ton format : numeroUnivers, adresseIp
    query.prepare("INSERT INTO UNIVERS (numeroUnivers, adresseIp) VALUES (:num, :ip)");
    query.bindValue(":num", numero);
    query.bindValue(":ip", ip);

    if(query.exec()) {
        succes = true;
    } else {
        qDebug() << "Erreur SQL (Insert Univers):" << query.lastError().text();
    }

    return succes;
}

bool AccessBDD::modifierUnivers(int id, int numero, const QString &ip)
{
    bool succes = false;
    QSqlQuery query;
    query.prepare("UPDATE UNIVERS SET numeroUnivers = :num, adresseIp = :ip WHERE idUnivers = :id");
    query.bindValue(":num", numero);
    query.bindValue(":ip", ip);
    query.bindValue(":id", id);

    if(query.exec()) {
        succes = true;
    } else {
        qDebug() << "Erreur SQL (Update Univers):" << query.lastError().text();
    }

    return succes;
}

bool AccessBDD::supprimerUnivers(int id)
{
    bool succes = false;
    QSqlQuery query;
    query.prepare("DELETE FROM UNIVERS WHERE idUnivers = :id");
    query.bindValue(":id", id);

    if(query.exec()) {
        succes = true;
    } else {
        // Souvent une erreur ici car l'ID est utilisé comme clé étrangère ailleurs (#idUnivers dans EQUIPEMENTS)
        qDebug() << "Erreur SQL (Delete Univers):" << query.lastError().text();
    }

    return succes;
}

void AccessBDD::chargerUneScène()
{

}

void AccessBDD::chargerLesScènes()
{

}

bool AccessBDD::enregistrerEquipment(const EquipmentData &eq, int idUniversSelectionne)
{
    bool succes = false;
    if (!bdd.transaction()) return false;

    QSqlQuery query;
    query.prepare("INSERT INTO EQUIPEMENTS (nomEquipement, adresseDepart, nbCanal, idUnivers) "
                  "VALUES (:nom, :adr, :nb, :idU)");
    query.bindValue(":nom", eq.nom);
    query.bindValue(":adr", eq.dmxStart.toInt());
    query.bindValue(":nb", eq.canaux.size());
    query.bindValue(":idU", idUniversSelectionne);

    if (query.exec()) {
        int idEq = query.lastInsertId().toInt();
        bool erreurCanal = false;

        for (int i = 0; i < eq.canaux.size(); ++i) {
            QSqlQuery qChan;
            qChan.prepare("INSERT INTO CANAUX (numeroCanal, description, idEquipement) VALUES (:num, :desc, :idEq)");
            qChan.bindValue(":num", i + 1);
            qChan.bindValue(":desc", eq.canaux[i].description);
            qChan.bindValue(":idEq", idEq);

            if (qChan.exec()) {
                int idC = qChan.lastInsertId().toInt();
                for (const auto& f : eq.canaux[i].fonctions) {
                    QSqlQuery qF;
                    qF.prepare("INSERT INTO FONCTIONNALITE_CANAL (valeurMin, valeurMax, fonction, idCanal) VALUES (:min, :max, :fct, :idC)");
                    qF.bindValue(":min", f.min.toInt());
                    qF.bindValue(":max", f.max.toInt());
                    qF.bindValue(":fct", f.nom);
                    qF.bindValue(":idC", idC);
                    if (!qF.exec()) {
                        qDebug() << "Erreur Fonctions:" << qF.lastError().text();
                        erreurCanal = true;
                    }
                }
            } else {
                qDebug() << "Erreur Canal:" << qChan.lastError().text();
                erreurCanal = true;
            }
        }

        if (!erreurCanal && bdd.commit()) {
            succes = true;
        } else {
            bdd.rollback();
        }
    } else {
        // C'EST ICI QUE TU VERRAS LE PROBLÈME DANS LA CONSOLE
        qDebug() << "ERREUR INSERT EQUIPEMENT:" << query.lastError().text();
        bdd.rollback();
    }
    return succes;
}

bool AccessBDD::supprimerEquipment(int idEquipement)
{
    bool succes = false;
    QSqlQuery query;

    // Grâce aux relations du schéma, il faut supprimer en cascade.
    // Si ta BDD n'est pas en "ON DELETE CASCADE", il faut supprimer
    // les fonctionnalités, puis les canaux, puis l'équipement.

    query.prepare("DELETE FROM EQUIPEMENTS WHERE idEquipement = :id");
    query.bindValue(":id", idEquipement);

    if (query.exec()) {
        succes = true;
    }

    return succes;
}

bool AccessBDD::modifierEquipment(int idEquipement, const EquipmentData &eq, int idUniversSelectionne)
{
    bool succes = false;

    if (bdd.transaction()) {
        QSqlQuery query;
        // 1. Mise à jour des infos de base de l'équipement
        query.prepare("UPDATE EQUIPEMENTS SET nomEquipement = :nom, adresseDepart = :adr, "
                      "nbCanal = :nb, idUnivers = :idU WHERE idEquipement = :idEq");
        query.bindValue(":nom", eq.nom);
        query.bindValue(":adr", eq.dmxStart.toInt());
        query.bindValue(":nb", eq.canaux.size());
        query.bindValue(":idU", idUniversSelectionne);
        query.bindValue(":idEq", idEquipement);

        if (query.exec()) {
            // 2. Supprimer les anciens canaux (La BDD doit être en ON DELETE CASCADE)
            // Sinon, il faudra supprimer manuellement FONCTIONNALITE_CANAL d'abord.
            QSqlQuery qDel;
            qDel.prepare("DELETE FROM CANAUX WHERE idEquipement = :idEq");
            qDel.bindValue(":idEq", idEquipement);

            if (qDel.exec()) {
                bool erreurInsertion = false;
                // 3. Ré-insertion des canaux et fonctions
                for (int i = 0; i < eq.canaux.size(); ++i) {
                    QSqlQuery qChan;
                    qChan.prepare("INSERT INTO CANAUX (numeroCanal, description, idEquipement) VALUES (:n, :d, :ie)");
                    qChan.bindValue(":n", i + 1);
                    qChan.bindValue(":d", eq.canaux[i].description);
                    qChan.bindValue(":ie", idEquipement);

                    if (qChan.exec()) {
                        int idC = qChan.lastInsertId().toInt();
                        for (const auto& f : eq.canaux[i].fonctions) {
                            QSqlQuery qF;
                            qF.prepare("INSERT INTO FONCTIONNALITE_CANAL (valeurMin, valeurMax, fonction, idCanal) "
                                       "VALUES (:min, :max, :fct, :idC)");
                            qF.bindValue(":min", f.min.toInt());
                            qF.bindValue(":max", f.max.toInt());
                            qF.bindValue(":fct", f.nom);
                            qF.bindValue(":idC", idC);
                            if (!qF.exec()) erreurInsertion = true;
                        }
                    } else { erreurInsertion = true; }
                }

                if (!erreurInsertion && bdd.commit()) {
                    succes = true;
                } else {
                    bdd.rollback();
                }
            } else { bdd.rollback(); }
        } else { bdd.rollback(); }
    }
    return succes;
}

QList<EquipmentData> AccessBDD::chargerTousLesEquipements()
{
    QList<EquipmentData> liste;
    // On récupère les équipements et on joint l'univers pour avoir son numéro à l'affichage
    QSqlQuery query("SELECT E.idEquipement, E.nomEquipement, E.adresseDepart, U.numeroUnivers "
                    "FROM EQUIPEMENTS E "
                    "JOIN UNIVERS U ON E.idUnivers = U.idUnivers");

    while (query.next()) {
        EquipmentData eq;
        eq.idEquipement = query.value("idEquipement").toInt();
        eq.nom = query.value("nomEquipement").toString();
        eq.dmxStart = query.value("adresseDepart").toString();
        eq.univers = "Univers " + query.value("numeroUnivers").toString();

        // Récupération des canaux de cet équipement
        QSqlQuery qChan;
        qChan.prepare("SELECT idCanal, description FROM CANAUX WHERE idEquipement = :idEq ORDER BY numeroCanal");
        qChan.bindValue(":idEq", eq.idEquipement);

        if (qChan.exec()) {
            while (qChan.next()) {
                ChannelData chan;
                chan.description = qChan.value("description").toString();
                int idC = qChan.value("idCanal").toInt();

                // Récupération des fonctionnalités de ce canal
                QSqlQuery qFunc;
                qFunc.prepare("SELECT valeurMin, valeurMax, fonction FROM FONCTIONNALITE_CANAL WHERE idCanal = :idC");
                qFunc.bindValue(":idC", idC);

                if (qFunc.exec()) {
                    while (qFunc.next()) {
                        FunctionData f;
                        f.nom = qFunc.value("fonction").toString();
                        f.min = qFunc.value("valeurMin").toString();
                        f.max = qFunc.value("valeurMax").toString();
                        chan.fonctions.append(f);
                    }
                }
                eq.canaux.append(chan);
            }
        }
        liste.append(eq);
    }
    return liste;
}
