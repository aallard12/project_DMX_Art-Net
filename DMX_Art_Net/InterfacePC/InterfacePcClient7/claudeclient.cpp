#include "claudeclient.h"

ClaudeClient::ClaudeClient(QObject* parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
}

void ClaudeClient::analyserFicheTechnique(const QString& contenuTexte)
{
    // ==========================================
    // 1. FILTRAGE INTELLIGENT DE LA FICHE TECHNIQUE (Gain énorme de tokens)
    // ==========================================
    QStringList lignes = contenuTexte.split('\n');
    QStringList lignesUtiles;

    // Expressions régulières pour cibler uniquement ce qui ressemble à un tableau DMX
    QRegularExpression regexDmx("(canal|channel|dmx|mode|val|min|max|fonction|function|\\bch\\b|\\d+)", QRegularExpression::CaseInsensitiveOption);
    QRegularExpression regexCaractereBizarre("[^\\x20-\\x7E\\x0A\\x0D\\x{00C0}-\\x{00FF}]");

    for (QString ligne : lignes) {
        ligne = ligne.trimmed();
        if (ligne.isEmpty()) continue;

        // Nettoyage des caractères corrompus
        ligne.replace(regexCaractereBizarre, " ");

        // On ne garde la ligne que si elle contient un mot-clé DMX OU un nombre (les plages de valeurs)
        if (ligne.contains(regexDmx)) {
            lignesUtiles.append(ligne);
        }
    }

    // Reconstruction du texte filtré
    QString texteFiltre = lignesUtiles.join("\n");

    // Sécurité ultime : Si le document est encore trop gigantesque, on le coupe (ex: 8000 caractères max)
    if (texteFiltre.length() > 8000) {
        texteFiltre = texteFiltre.left(8000) + "\n[... Fiche tronquée pour économie de tokens ...]";
    }

    // ==========================================
    // 2. PROMPT ULTRA-CONDENSÉ (ÉCONOMIE DE TOKENS)
    // ==========================================
    QString prompt = QString(
        "Extract DMX datasheet to JSON object. Rules: valid JSON only, no markdown, skip corrupt text.\n"
        "Structure:\n"
        "{\n"
        "  \"equipement\": {\n"
        "    \"nom\": \"\", \"nb_canaux\": \"\",\n"
        "    \"canaux\": [\n"
        "      { \"numero\": \"1\", \"description\": \"\", \"fonctions\": [{ \"nom\": \"\", \"min\": \"0\", \"max\": \"255\" }] }\n"
        "    ]\n"
        "  }\n"
        "}\n\n"
        "Data:\n%1"
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
