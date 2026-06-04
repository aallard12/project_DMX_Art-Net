/**
 * @file mistralclient.h
 * @brief Déclaration de la classe MistralClient
 * @author Antoine ALLARD
 * @date 10/04/2026
 */

#ifndef MISTRALCLIENT_H
#define MISTRALCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class MistralClient : public QObject
{
    Q_OBJECT

public:
    explicit MistralClient(QObject* parent = nullptr);
    void analyserFicheTechnique(const QString& contenuTexte);

signals:
    void analyseTerminee(const QJsonObject& equipementJson);
    void erreur(const QString& message);

private:
    QNetworkAccessManager* manager;
    const QString API_KEY  = "GvYgl3wndh3yvEcyil55H1W3Edn4SIap";
    const QString API_URL  = "https://api.mistral.ai/v1/chat/completions";
};

#endif
