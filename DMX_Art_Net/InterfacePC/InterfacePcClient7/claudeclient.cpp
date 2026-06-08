#include "claudeclient.h"

ClaudeClient::ClaudeClient(QObject* parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
}

void ClaudeClient::analyserFicheTechnique(const QString& contenuTexte)
{
    QRegularExpression regexCaractereBizarre("[^\\x20-\\x7E\\x0A\\x0D\\x{00C0}-\\x{00FF}]");
    QStringList lignes = contenuTexte.split('\n');
    QStringList lignesUtiles;

    for (QString ligne : lignes) {
        ligne = ligne.trimmed();
        if (ligne.isEmpty()) continue;
        ligne.replace(regexCaractereBizarre, " ");
        lignesUtiles.append(ligne);
    }

    // Cherche le début de la section DMX dans le document
    QRegularExpression regexSectionDmx(
                "(dmx|canal|channel|configuration|mapping|fixture|mode\\s*\\d+\\s*ch)",
                QRegularExpression::CaseInsensitiveOption
                );

    int indexDebutDmx = -1;
    for (int i = 0; i < lignesUtiles.size(); ++i) {
        if (lignesUtiles[i].contains(regexSectionDmx)) {
            indexDebutDmx = i;
            break;
        }
    }

    // Si on trouve une section DMX, on part de là — sinon on prend tout
    QStringList lignesDmx = (indexDebutDmx != -1)
            ? lignesUtiles.mid(indexDebutDmx)
            : lignesUtiles;

    QString texteFiltre = lignesDmx.join("\n");

    if (texteFiltre.length() > 8000)
        texteFiltre = texteFiltre.left(8000) + "\n[... Fiche tronquée ...]";

    QString prompt = QString(
                "You are a DMX lighting expert. Your task is to extract the DMX channel table.\n"
                "CRITICAL RULES:\n"
                "- Valid JSON only, absolutely no markdown, no explanation outside JSON\n"
                "- The 'description' field = the channel's PURPOSE (e.g. 'Rouge', 'Pan', 'Dimmer', 'Strobe')\n"
                "- NEVER use 'Channel 1', 'Channel 2' as description — use the actual function name\n"
                "- NEVER use generic 'Dimmer' for every channel — read each channel individually\n"
                "- Each function must have the EXACT min/max values from the table\n"
                "- If a channel has multiple value ranges, create one function entry per range\n"
                "- Pick the most complete DMX mode (most channels)\n"
                "Structure:\n"
                "{\n"
                "  \"equipement\": {\n"
                "    \"nom\": \"\", \"nb_canaux\": \"\",\n"
                "    \"canaux\": [\n"
                "      { \"numero\": \"1\", \"description\": \"Rouge\", \"fonctions\": [{ \"nom\": \"Intensite Rouge\", \"min\": \"0\", \"max\": \"255\" }] },\n"
                "      { \"numero\": \"4\", \"description\": \"Dimmer\", \"fonctions\": [{ \"nom\": \"Blackout\", \"min\": \"0\", \"max\": \"9\" }, { \"nom\": \"Dimmer\", \"min\": \"10\", \"max\": \"255\" }] }\n"
                "    ]\n"
                "  }\n"
                "}\n\nDatasheet:\n%1"
                ).arg(texteFiltre);

    QJsonObject message;
    message["role"]    = "user";
    message["content"] = prompt;

    QJsonObject body;
    body["model"]       = "llama-3.3-70b-versatile";
    body["temperature"] = 0.1;
    body["messages"]    = QJsonArray{ message };

    QJsonObject responseFormat;
    responseFormat["type"] = "json_object";
    body["response_format"] = responseFormat;

    QUrl url(API_URL);
    QNetworkRequest request;
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(API_KEY).toUtf8());

    QNetworkReply* reply = manager->post(request, QJsonDocument(body).toJson());

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            QJsonArray choicesArray = doc.object()["choices"].toArray();

            if (!choicesArray.isEmpty()) {
                QString contenu = choicesArray[0].toObject()["message"].toObject()["content"].toString();
                QJsonDocument result = QJsonDocument::fromJson(contenu.toUtf8());

                if (!result.isNull() && result.isObject()) {
                    emit analyseTerminee(result.object());
                } else {
                    emit erreur("La réponse de Groq n'est pas un JSON valide");
                }
            } else {
                emit erreur("Format de réponse inattendu de l'API Groq");
            }
        } else {
            QByteArray errorBody = reply->bytesAvailable() ? reply->readAll() : reply->errorString().toUtf8();
            qDebug() << "Détails de l'erreur Groq :" << errorBody;
            emit erreur("Erreur réseau (Quota/Taille dépassée ou clé invalide)");
        }
        reply->deleteLater();
    });
}
