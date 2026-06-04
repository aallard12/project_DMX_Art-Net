#include "mistralclient.h"

MistralClient::MistralClient(QObject* parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
}

void MistralClient::analyserFicheTechnique(const QString& contenuTexte)
{
    QString prompt = QString(
                "Tu es un expert DMX. Analyse cette fiche technique et retourne UNIQUEMENT un JSON "
                "strictement dans ce format, sans texte avant ni après :\n"
                "{\n"
                "  \"equipement\": {\n"
                "    \"nom\": \"...\",\n"
                "    \"nb_canaux\": \"...\",\n"
                "    \"canaux\": [\n"
                "      {\n"
                "        \"numero\": \"1\",\n"
                "        \"description\": \"...\",\n"
                "        \"fonctions\": [\n"
                "          { \"nom\": \"...\", \"min\": \"0\", \"max\": \"255\" }\n"
                "        ]\n"
                "      }\n"
                "    ]\n"
                "  }\n"
                "}\n\nFiche technique :\n%1"
                ).arg(contenuTexte);

    QJsonObject message;
    message["role"]    = "user";
    message["content"] = prompt;

    QJsonObject body;
    body["model"]       = "mistral-small-latest";
    body["temperature"] = 0.1;
    body["messages"]    = QJsonArray{ message };

    QUrl url(API_URL);
    QNetworkRequest request;
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(API_KEY).toUtf8());

    QNetworkReply* reply = manager->post(request, QJsonDocument(body).toJson());

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            QString contenu   = doc.object()["choices"].toArray()[0]
                    .toObject()["message"]
                    .toObject()["content"].toString();
            QJsonDocument result = QJsonDocument::fromJson(contenu.toUtf8());
            if (!result.isNull() && result.isObject())
                emit analyseTerminee(result.object());
            else
                emit erreur("La réponse de Mistral n'est pas un JSON valide");
        } else {
            QByteArray errorBody = reply->readAll();
            qDebug() << "Détails de l'erreur Mistral :" << errorBody;
            emit erreur("Erreur réseau : " + reply->errorString());
        }
        reply->deleteLater();
    });
}
