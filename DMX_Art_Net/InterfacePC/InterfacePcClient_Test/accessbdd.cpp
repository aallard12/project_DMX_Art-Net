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

    if (bdd.transaction()) {
        QSqlQuery query;

        // 1. Supprimer les fonctionnalités liées aux canaux de cet équipement
        query.prepare("DELETE FROM FONCTIONNALITE_CANAL WHERE idCanal IN "
                      "(SELECT idCanal FROM CANAUX WHERE idEquipement = :idEq)");
        query.bindValue(":idEq", idEquipement);
        bool okFunc = query.exec();

        // 2. Supprimer les canaux liés à l'équipement
        query.prepare("DELETE FROM CANAUX WHERE idEquipement = :idEq");
        query.bindValue(":idEq", idEquipement);
        bool okChan = query.exec();

        // 3. Supprimer l'équipement lui-même
        query.prepare("DELETE FROM EQUIPEMENTS WHERE idEquipement = :idEq");
        query.bindValue(":idEq", idEquipement);
        bool okEq = query.exec();

        if (okFunc && okChan && okEq && bdd.commit()) {
            succes = true;
        } else {
            qDebug() << "Erreur suppression cascade:" << query.lastError().text();
            bdd.rollback();
        }
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

QMap<int, DmxChannelInfo> AccessBDD::chargerMapUnivers(int idUnivers)
{
    QMap<int, DmxChannelInfo> map;
    QSqlQuery query;

    // 1. On récupère les canaux comme avant
    query.prepare("SELECT C.idCanal, C.numeroCanal, C.description, E.adresseDepart, E.nomEquipement "
                  "FROM CANAUX C "
                  "JOIN EQUIPEMENTS E ON C.idEquipement = E.idEquipement "
                  "WHERE E.idUnivers = :idU");
    query.bindValue(":idU", idUnivers);

    if (query.exec()) {
        while (query.next()) {
            int addrDepart = query.value("adresseDepart").toInt();
            int numCanal = query.value("numeroCanal").toInt();
            int canalAbsolu = addrDepart + numCanal - 1;

            DmxChannelInfo info;
            info.idCanal = query.value("idCanal").toInt();
            info.nomEquipement = query.value("nomEquipement").toString();
            info.description = query.value("description").toString();

            map.insert(canalAbsolu, info);
        }
    } else {
        qDebug() << "Erreur Map Univers:" << query.lastError().text();
    }

    // 2. NOUVEAU : On charge les fonctionnalités pour chaque canal trouvé
    for (auto it = map.begin(); it != map.end(); ++it) {
        QSqlQuery qFunc;
        qFunc.prepare("SELECT valeurMin, valeurMax, fonction FROM FONCTIONNALITE_CANAL WHERE idCanal = :idC");
        qFunc.bindValue(":idC", it.value().idCanal);

        if (qFunc.exec()) {
            while (qFunc.next()) {
                DmxFunctionInfo f;
                f.min = qFunc.value("valeurMin").toInt();
                f.max = qFunc.value("valeurMax").toInt();
                f.nom = qFunc.value("fonction").toString();
                it.value().fonctions.append(f);
            }
        }
    }

    return map;
}

bool AccessBDD::enregistrerScene(const QString &nomScene, const QMap<int, int> &valeursCanaux)
{
    bool succes = false;

    if (bdd.transaction()) {
        QSqlQuery query;
        // 1. Création de la scène
        query.prepare("INSERT INTO SCENES (nomScene) VALUES (:nom)");
        query.bindValue(":nom", nomScene);

        if (query.exec()) {
            int idScene = query.lastInsertId().toInt();
            bool erreurPilote = false;

            // 2. Insertion dans PILOTE (uniquement les valeurs filtrées)
            for (auto it = valeursCanaux.begin(); it != valeursCanaux.end(); ++it) {
                QSqlQuery qPilote;
                qPilote.prepare("INSERT INTO PILOTE (idScene, idCanal, valeurCanaux) "
                                "VALUES (:idS, :idC, :val)");
                qPilote.bindValue(":idS", idScene);
                qPilote.bindValue(":idC", it.key());   // it.key() contient l'idCanal
                qPilote.bindValue(":val", it.value()); // it.value() contient la valeur > 0

                if (!qPilote.exec()) {
                    qDebug() << "Erreur PILOTE:" << qPilote.lastError().text();
                    erreurPilote = true;
                    break; // Inutile de continuer si une ligne plante
                }
            }

            if (!erreurPilote && bdd.commit()) {
                succes = true;
            } else {
                bdd.rollback();
            }
        } else {
            qDebug() << "Erreur création SCENE:" << query.lastError().text();
            bdd.rollback();
        }
    }

    return succes;
}

QList<SceneData> AccessBDD::chargerLesScenes()
{
    QList<SceneData> liste;
    QSqlQuery query("SELECT idScene, nomScene FROM SCENES ORDER BY nomScene ASC");

    while(query.next()) {
        SceneData s;
        s.idScene = query.value("idScene").toInt();
        s.nomScene = query.value("nomScene").toString();
        liste.append(s);
    }
    return liste;
}

QMap<int, int> AccessBDD::chargerValeursScene(int idScene)
{
    QMap<int, int> map;
    QSqlQuery query;
    // On récupère uniquement les canaux et leurs valeurs pour cette scène précise
    query.prepare("SELECT idCanal, valeurCanaux FROM PILOTE WHERE idScene = :idS");
    query.bindValue(":idS", idScene);

    if(query.exec()) {
        while(query.next()) {
            map.insert(query.value("idCanal").toInt(), query.value("valeurCanaux").toInt());
        }
    } else {
        qDebug() << "Erreur Chargement Valeurs Scène:" << query.lastError().text();
    }
    return map;
}

int AccessBDD::getUniversDeScene(int idScene)
{
    QSqlQuery query;
    // On cherche le numéro de l'univers associé aux canaux de cette scène
    query.prepare("SELECT U.numeroUnivers FROM PILOTE P "
                  "JOIN CANAUX C ON P.idCanal = C.idCanal "
                  "JOIN EQUIPEMENTS E ON C.idEquipement = E.idEquipement "
                  "JOIN UNIVERS U ON E.idUnivers = U.idUnivers "
                  "WHERE P.idScene = :idS LIMIT 1");
    query.bindValue(":idS", idScene);

    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }

    // Si la scène est vide ou qu'il y a un problème, on retourne l'univers 1 par défaut
    return 1;
}

bool AccessBDD::renommerScene(int idScene, const QString& nouveauNom) {
    QSqlQuery query;
    query.prepare("UPDATE SCENES SET nomScene = :nom WHERE idScene = :id");
    query.bindValue(":nom", nouveauNom);
    query.bindValue(":id", idScene);
    return query.exec();
}

bool AccessBDD::supprimerScene(int idScene) {
    if (bdd.transaction()) {
        QSqlQuery q1, q2;
        // Supprimer les valeurs des canaux d'abord
        q1.prepare("DELETE FROM PILOTE WHERE idScene = :id");
        q1.bindValue(":id", idScene);

        // Supprimer la scène ensuite
        q2.prepare("DELETE FROM SCENES WHERE idScene = :id");
        q2.bindValue(":id", idScene);

        if (q1.exec() && q2.exec()) {
            return bdd.commit();
        }
        bdd.rollback();
    }
    return false;
}
