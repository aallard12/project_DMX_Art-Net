/**
 * @file claudeclient.h
 * @brief Déclaration de la classe ClaudeClient
 * @author Antoine ALLARD
 * @date 10/04/2026
 */

#ifndef CLAUDECLIENT_H
#define CLAUDECLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class ClaudeClient : public QObject
{
    Q_OBJECT

public:
    explicit ClaudeClient(QObject* parent = nullptr);
    void analyserFicheTechnique(const QString& contenuTexte);

signals:
    void analyseTerminee(const QJsonObject& equipementJson);
    void erreur(const QString& message);

private:
    QNetworkAccessManager* manager;
    const QString API_KEY     = "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"; // À remplacer par ta véritable clé
    const QString API_URL     = "https://api.groq.com/openai/v1/chat/completions";
};

#endif
