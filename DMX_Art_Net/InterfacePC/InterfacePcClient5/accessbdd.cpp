/**
 * @file accessbdd.cpp
 * @brief Implémentation de la classe AccessBDD
 * @author Antoine ALLARD
 * @date 10/04/2026
 * @details Classe modélisant la communication avec la base de données
 */

#include "accessbdd.h"

/**
 * @brief AccessBDD::AccessBDD
 * @details Initialise la connexion à la base de données. Tente de lire les paramètres
 * de connexion (IP, nom de base, utilisateur, mot de passe) depuis un fichier "commandes.ini".
 * Si le fichier est absent, des valeurs par défaut sont utilisées. La connexion est ouverte
 * dès l'instanciation.
 */
AccessBDD::AccessBDD() {
    bdd = QSqlDatabase::addDatabase("QSQLITE");
    bdd.setDatabaseName("dmxbdd.sqlite");

    if (!bdd.open()) {
        qDebug() << bdd.lastError().text();
    } else {
        QSqlQuery("PRAGMA foreign_keys = ON", bdd);
        qDebug() << "Chemin DB :" << bdd.databaseName();
        qDebug() << "Fichier SQL trouvé :" << QFile::exists("bdd_sqlite.sql");
        // initialiserSchema();

        qDebug() << "BDD OK";
        QSqlQuery test("SELECT COUNT(*) FROM UNIVERS", bdd);
        if (test.next()) qDebug() << "Nb univers en base :" << test.value(0).toInt();
    }

    // bdd = QSqlDatabase::addDatabase("QMYSQL");
    // QString ip, base, log, mdp;
    // QString nomFichierIni = "commandes.ini";
    // QFileInfo testFichier(nomFichierIni);

    // if (testFichier.exists() && testFichier.isFile()) {
    //     QSettings paramsSocket(nomFichierIni, QSettings::IniFormat);
    //     ip = paramsSocket.value("CONFIG/hostname", "192.168.1.20").toString();
    //     base = paramsSocket.value("CONFIG/BDD", "DMXBDD").toString();
    //     log = paramsSocket.value("CONFIG/username", "root").toString();
    //     mdp = paramsSocket.value("CONFIG/password", "raspberry").toString();
    // }

    // // if (testFichier.exists() && testFichier.isFile()) {
    // //     QSettings paramsSocket(nomFichierIni, QSettings::IniFormat);
    // //     ip = paramsSocket.value("CONFIG/hostname", "172.18.58.8").toString();
    // //     base = paramsSocket.value("CONFIG/BDD", "DMXBDD").toString();
    // //     log = paramsSocket.value("CONFIG/username", "ciel").toString();
    // //     mdp = paramsSocket.value("CONFIG/password", "ciel").toString();
    // // }

    // bdd.setHostName(ip);
    // bdd.setDatabaseName(base);
    // bdd.setUserName(log);
    // bdd.setPassword(mdp);

    // if (!bdd.open())
    //     qDebug() << bdd.lastError().text();
    // else
    //     qDebug() << "BDD OK";
}

void AccessBDD::initialiserSchema() {
    QFile f("bdd_sqlite.sql");
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Impossible d'ouvrir bdd_sqlite.sql :" << f.errorString();
        return;
    }

    QSqlQuery query(bdd);
    QString sql = f.readAll();

    // Supprime les commentaires -- avant de splitter
    QStringList lignes = sql.split('\n');
    QStringList lignesSansCommentaires;
    for (const QString& l : lignes) {
        QString trimmed = l.trimmed();
        if (!trimmed.startsWith("--") && !trimmed.isEmpty())
            lignesSansCommentaires << l;
    }
    sql = lignesSansCommentaires.join('\n');

    for (const QString& req : sql.split(";", Qt::SkipEmptyParts)) {
        QString trimmed = req.trimmed();
        if (!trimmed.isEmpty()) {
            if (!query.exec(trimmed))
                qDebug() << "Erreur SQL:" << query.lastError().text() << "|" << trimmed.left(60);
        }
    }
}

/**
 * @brief AccessBDD::chargerUnivers
 * @details Exécute une requête SELECT pour récupérer la liste de tous les univers
 * enregistrés en base, triés par numéro ascendant.
 * @return QList<UniversData> Liste structurée des univers trouvés.
 */
QList<UniversData> AccessBDD::chargerUnivers() {
    QList<UniversData> liste;
    QSqlQuery query("SELECT idUnivers, numeroUnivers, adresseIp FROM UNIVERS ORDER BY numeroUnivers ASC");
    while(query.next()) {
        UniversData u;
        u.idUnivers = query.value("idUnivers").toInt();
        u.numero = query.value("numeroUnivers").toInt();
        u.ip = query.value("adresseIp").toString();
        liste.append(u);
    }
    return liste;
}

/**
 * @brief AccessBDD::enregistrerUnivers
 * @details Insère un nouvel univers dans la table UNIVERS après avoir vérifié
 * la validité des paramètres (IP non vide et numéro positif).
 * @param numero Le numéro identifiant l'univers.
 * @param ip L'adresse IP associée au contrôleur de cet univers.
 * @return bool True si l'insertion a réussi, sinon false.
 */
bool AccessBDD::enregistrerUnivers(int numero, const QString &ip) {
    bool succes = false;
    if (!ip.trimmed().isEmpty() && numero>0) {
        QSqlQuery query;
        query.prepare("INSERT INTO UNIVERS (numeroUnivers, adresseIp) VALUES (:num, :ip)");
        query.bindValue(":num", numero);
        query.bindValue(":ip", ip);
        if (query.exec()) {
            succes = true;
        }
    }
    return succes;
}

/**
 * @brief AccessBDD::modifierUnivers
 * @details Met à jour les informations (numéro et IP) d'un univers existant
 * identifié par son ID unique.
 * @param id Identifiant unique (PK) en base de données.
 * @param numero Nouveau numéro d'univers.
 * @param ip Nouvelle adresse IP.
 * @return bool True si la mise à jour a réussi.
 */
bool AccessBDD::modifierUnivers(int id, int numero, const QString &ip) {
    QSqlQuery query;
    query.prepare("UPDATE UNIVERS SET numeroUnivers = :num, adresseIp = :ip WHERE idUnivers = :id");
    query.bindValue(":num", numero);
    query.bindValue(":ip", ip);
    query.bindValue(":id", id);
    return query.exec();
}

/**
 * @brief AccessBDD::supprimerUnivers
 * @details Supprime définitivement un univers de la base de données.
 * Attention : selon les contraintes de clés étrangères, cela peut échouer si
 * des équipements y sont liés.
 * @param id Identifiant de l'univers à supprimer.
 * @return bool True si la suppression est effective.
 */
bool AccessBDD::supprimerUnivers(int id) {
    QSqlQuery query;
    query.prepare("DELETE FROM UNIVERS WHERE idUnivers = :id");
    query.bindValue(":id", id);
    return query.exec();
}

/**
 * @brief AccessBDD::enregistrerEquipment
 * @details Procédure complexe utilisant une **transaction SQL**. Elle enregistre
 * l'équipement, puis itère sur ses canaux pour les insérer, et enfin insère les
 * fonctionnalités (plages de valeurs) pour chaque canal. En cas d'erreur à n'importe
 * quelle étape, un rollback est effectué pour garantir l'intégrité des données.
 * @param eq Structure contenant toutes les données de l'équipement.
 * @param idUniversSelectionne ID de l'univers auquel l'équipement appartient.
 * @return bool True si l'intégralité de la structure a été sauvegardée.
 */
bool AccessBDD::enregistrerEquipment(const EquipmentData &eq, int idUniversSelectionne) {
    bool succes = false;

    if (bdd.isOpen() && bdd.transaction()) {
        QSqlQuery query;
        query.prepare("INSERT INTO EQUIPEMENTS (nomEquipement, adresseDepart, nbCanal, idUnivers, couleur) "
                      "VALUES (:nom, :adr, :nb, :idU, :couleur)");
        query.bindValue(":nom", eq.nom);
        query.bindValue(":adr", eq.dmxStart.toInt());
        query.bindValue(":nb", eq.canaux.size());
        query.bindValue(":idU", idUniversSelectionne);
        query.bindValue(":couleur", eq.couleur);

        if (query.exec()) {
            int idEquip = query.lastInsertId().toInt();
            bool errorOccured = false;

            for (int i = 0; i < eq.canaux.size() && !errorOccured; ++i) {
                QSqlQuery qChan;
                qChan.prepare("INSERT INTO CANAUX (numeroCanal, description, idEquipement) "
                              "VALUES (:num, :desc, :idE)");
                qChan.bindValue(":num", eq.dmxStart.toInt() + i);
                qChan.bindValue(":desc", eq.canaux[i].description);
                qChan.bindValue(":idE", idEquip);

                if (qChan.exec()) {
                    int idChan = qChan.lastInsertId().toInt();
                    for (const auto& func : eq.canaux[i].fonctions) {
                        QSqlQuery qFunc;
                        qFunc.prepare("INSERT INTO FONCTIONNALITE_CANAL (valeurMin, valeurMax, fonction, idCanal) "
                                      "VALUES (:min, :max, :f, :idC)");
                        qFunc.bindValue(":min", func.min.toInt());
                        qFunc.bindValue(":max", func.max.toInt());
                        qFunc.bindValue(":f", func.nom);
                        qFunc.bindValue(":idC", idChan);
                        if (!qFunc.exec()) errorOccured = true;
                    }
                } else {
                    errorOccured = true;
                }
            }

            if (!errorOccured && bdd.commit()) {
                succes = true;
            } else {
                bdd.rollback();
            }
        } else {
            bdd.rollback();
        }
    }

    return succes;
}

/**
 * @brief AccessBDD::supprimerEquipment
 * @details Supprime un équipement via son ID. Les canaux et fonctionnalités associés
 * sont normalement supprimés par cascade si la BDD est configurée ainsi.
 * @param idEquipement ID de l'équipement.
 * @return bool True si la requête a abouti.
 */
bool AccessBDD::supprimerEquipment(int idEquipement) {
    QSqlQuery query;
    query.prepare("DELETE FROM EQUIPEMENTS WHERE idEquipement = :id");
    query.bindValue(":id", idEquipement);
    return query.exec();
}

/**
 * @brief AccessBDD::modifierEquipment
 * @details Met à jour un équipement en utilisant une transaction. La méthode met à jour
 * les infos de base, puis supprime tous les anciens canaux pour réinsérer la nouvelle
 * configuration (canaux et fonctionnalités). Utilise commit/rollback pour la sécurité.
 * @param idEquipement ID de l'équipement à modifier.
 * @param eq Nouvelles données de l'équipement.
 * @param idUniversSelectionne ID du nouvel univers (si changé).
 * @return bool True si la modification totale est réussie.
 */
bool AccessBDD::modifierEquipment(int idEquipement, const EquipmentData &eq, int idUniversSelectionne) {
    bool succes = false;

    if (bdd.isOpen() && bdd.transaction()) {
        QSqlQuery query;
        query.prepare("UPDATE EQUIPEMENTS SET nomEquipement = :nom, adresseDepart = :adr, "
                      "nbCanal = :nb, idUnivers = :idU, couleur = :couleur WHERE idEquipement = :idEq");
        query.bindValue(":nom", eq.nom);
        query.bindValue(":adr", eq.dmxStart.toInt());
        query.bindValue(":nb", eq.canaux.size());
        query.bindValue(":idU", idUniversSelectionne);
        query.bindValue(":couleur", eq.couleur);
        query.bindValue(":idEq", idEquipement);

        if (query.exec()) {
            QSqlQuery qDel;
            qDel.prepare("DELETE FROM CANAUX WHERE idEquipement = :idEq");
            qDel.bindValue(":idEq", idEquipement);

            if (qDel.exec()) {
                bool errorOccured = false;

                for (int i = 0; i < eq.canaux.size() && !errorOccured; ++i) {
                    QSqlQuery qChan;
                    qChan.prepare("INSERT INTO CANAUX (numeroCanal, description, idEquipement) "
                                  "VALUES (:num, :desc, :idE)");
                    qChan.bindValue(":num", eq.dmxStart.toInt() + i);
                    qChan.bindValue(":desc", eq.canaux[i].description);
                    qChan.bindValue(":idE", idEquipement);

                    if (qChan.exec()) {
                        int idChan = qChan.lastInsertId().toInt();
                        for (const auto& func : eq.canaux[i].fonctions) {
                            QSqlQuery qFunc;
                            qFunc.prepare("INSERT INTO FONCTIONNALITE_CANAL (valeurMin, valeurMax, fonction, idCanal) "
                                          "VALUES (:min, :max, :f, :idC)");
                            qFunc.bindValue(":min", func.min.toInt());
                            qFunc.bindValue(":max", func.max.toInt());
                            qFunc.bindValue(":f", func.nom);
                            qFunc.bindValue(":idC", idChan);
                            if (!qFunc.exec()) errorOccured = true;
                        }
                    } else {
                        errorOccured = true;
                    }
                }

                if (!errorOccured && bdd.commit()) {
                    succes = true;
                } else {
                    bdd.rollback();
                }
            } else {
                bdd.rollback();
            }
        } else {
            bdd.rollback();
        }
    }

    return succes;
}

/**
 * @brief AccessBDD::chargerTousLesEquipements
 * @details Récupère l'intégralité des équipements de la base avec leurs canaux
 * et leurs fonctionnalités. Effectue des requêtes imbriquées pour reconstruire
 * l'arborescence complète des objets EquipmentData.
 * @return QList<EquipmentData> Liste complète des équipements détaillés.
 */
QList<EquipmentData> AccessBDD::chargerTousLesEquipements() {
    QList<EquipmentData> liste;
    QSqlQuery query("SELECT E.idEquipement, E.nomEquipement, E.adresseDepart, E.couleur, U.numeroUnivers "
                    "FROM EQUIPEMENTS E JOIN UNIVERS U ON E.idUnivers = U.idUnivers");

    while (query.next()) {
        EquipmentData eq;
        eq.idEquipement = query.value("idEquipement").toInt();
        eq.nom = query.value("nomEquipement").toString();
        eq.dmxStart = query.value("adresseDepart").toString();
        eq.univers = "Univers " + query.value("numeroUnivers").toString();
        eq.couleur = query.value("couleur").toString();

        QSqlQuery qChan;
        qChan.prepare("SELECT idCanal, description FROM CANAUX WHERE idEquipement = :idEq ORDER BY numeroCanal");
        qChan.bindValue(":idEq", eq.idEquipement);

        if (qChan.exec()) {
            while (qChan.next()) {
                ChannelData chan;
                chan.description = qChan.value("description").toString();
                int idC = qChan.value("idCanal").toInt();

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

/**
 * @brief AccessBDD::chargerMapUnivers
 * @details Construit une cartographie DMX pour un univers donné. Calcule le
 * canal absolu (`adresseDepart + numeroCanal - 1`) pour chaque canal et
 * l'associe à ses informations (nom équipement, couleur, fonctions).
 * @param idUnivers ID de l'univers à charger.
 * @return QMap<int, DmxChannelInfo> Map indexée par le canal DMX (1-512).
 */
QMap<int, DmxChannelInfo> AccessBDD::chargerMapUnivers(int idUnivers) {
    QMap<int, DmxChannelInfo> map;
    QSqlQuery query;
    query.prepare("SELECT C.idCanal, C.numeroCanal, C.description, E.adresseDepart, E.nomEquipement, E.couleur "
                  "FROM CANAUX C JOIN EQUIPEMENTS E ON C.idEquipement = E.idEquipement "
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
            info.couleur = query.value("couleur").toString();
            info.description = query.value("description").toString();
            map.insert(canalAbsolu, info);
        }
    }

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

/**
 * @brief AccessBDD::enregistrerScene
 * @details Crée une nouvelle scène dans la table SCENES, puis enregistre l'état
 * de tous les canaux actifs associés dans la table PILOTE via une transaction.
 * @param nomScene Nom donné à la scène.
 * @param valeursCanaux Map contenant l'ID du canal et sa valeur DMX (0-255).
 * @return bool True si la scène et ses réglages sont sauvegardés.
 */
bool AccessBDD::enregistrerScene(const QString &nomScene, const QMap<int, int> &valeursCanaux) {
    bool succes = false;

    if (bdd.transaction()) {
        QSqlQuery query;
        query.prepare("INSERT INTO SCENES (nomScene) VALUES (:nom)");
        query.bindValue(":nom", nomScene);

        if (query.exec()) {
            int idScene = query.lastInsertId().toInt();
            bool erreur = false;

            for (auto it = valeursCanaux.begin(); it != valeursCanaux.end() && !erreur; ++it) {
                QSqlQuery qPilote;
                qPilote.prepare("INSERT INTO PILOTE (idScene, idCanal, valeurCanaux) VALUES (:idS, :idC, :val)");
                qPilote.bindValue(":idS", idScene);
                qPilote.bindValue(":idC", it.key());
                qPilote.bindValue(":val", it.value());
                if (!qPilote.exec()) erreur = true;
            }

            if (!erreur && bdd.commit()) {
                succes = true;
            } else {
                bdd.rollback();
            }
        } else {
            bdd.rollback();
        }
    }

    return succes;
}

/**
 * @brief AccessBDD::recupererCompteurCanaux
 * @details Compte le nombre total de canaux configurés pour un univers spécifique
 * en joignant les tables CANAUX et EQUIPEMENTS.
 * @param index ID de l'univers concerné.
 * @return int Nombre de canaux trouvés.
 */
int AccessBDD::recupererCompteurCanaux(int index)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(c.idCanal) AS nombreCanaux FROM CANAUX c "
                  "JOIN EQUIPEMENTS e ON c.idEquipement = e.idEquipement "
                  "WHERE e.idUnivers = :index; ");

    int countCanaux = 0;
    query.bindValue(":index", index);
    if (query.exec() && query.next()) {
        countCanaux = query.value("nombreCanaux").toInt();
    }
    return countCanaux;
}

/**
 * @brief AccessBDD::chargerLesScenes
 * @details Récupère la liste de toutes les scènes enregistrées, triées par nom.
 * @return QList<SceneData> Liste des scènes (ID et nom).
 */
QList<SceneData> AccessBDD::chargerLesScenes() {
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

/**
 * @brief AccessBDD::chargerValeursScene
 * @details Récupère les valeurs DMX enregistrées pour une scène spécifique.
 * @param idScene ID de la scène.
 * @return QMap<int, int> Map associant l'ID du canal à sa valeur enregistrée.
 */
QMap<int, int> AccessBDD::chargerValeursScene(int idScene) {
    QMap<int, int> map;
    QSqlQuery query;
    query.prepare("SELECT idCanal, valeurCanaux FROM PILOTE WHERE idScene = :idS");
    query.bindValue(":idS", idScene);
    if(query.exec()) {
        while(query.next())
            map.insert(query.value("idCanal").toInt(), query.value("valeurCanaux").toInt());
    }
    return map;
}

/**
 * @brief AccessBDD::getUniversDeScene
 * @details Identifie le numéro d'univers lié à une scène en remontant la chaîne
 * Pilote -> Canaux -> Equipements -> Univers. Retourne 1 par défaut si aucun n'est trouvé.
 * @param idScene ID de la scène.
 * @return int Numéro de l'univers correspondant.
 */
int AccessBDD::getUniversDeScene(int idScene) {
    QSqlQuery query;
    query.prepare("SELECT U.numeroUnivers FROM PILOTE P "
                  "JOIN CANAUX C ON P.idCanal = C.idCanal "
                  "JOIN EQUIPEMENTS E ON C.idEquipement = E.idEquipement "
                  "JOIN UNIVERS U ON E.idUnivers = U.idUnivers "
                  "WHERE P.idScene = :idS LIMIT 1");
    query.bindValue(":idS", idScene);
    return (query.exec() && query.next()) ? query.value(0).toInt() : 1;
}

/**
 * @brief AccessBDD::renommerScene
 * @details Change le nom d'une scène existante en base de données.
 * @param idScene ID de la scène.
 * @param nouveauNom Nouveau nom à appliquer.
 * @return bool True si le renommage a réussi.
 */
bool AccessBDD::renommerScene(int idScene, const QString& nouveauNom) {
    QSqlQuery query;
    query.prepare("UPDATE SCENES SET nomScene = :nom WHERE idScene = :id");
    query.bindValue(":nom", nouveauNom);
    query.bindValue(":id", idScene);
    return query.exec();
}

/**
 * @brief AccessBDD::supprimerScene
 * @details Supprime une scène de la table SCENES. La table PILOTE devrait être
 * nettoyée par cascade si configurée en BDD.
 * @param idScene ID de la scène à supprimer.
 * @return bool True si la suppression a réussi.
 */
bool AccessBDD::supprimerScene(int idScene) {
    bool succes = false;

    QSqlQuery query;
    query.prepare("DELETE FROM SCENES WHERE idScene = :id");
    query.bindValue(":id", idScene);
    if (query.exec()) {
        succes = true;
    }
    return succes;
}
