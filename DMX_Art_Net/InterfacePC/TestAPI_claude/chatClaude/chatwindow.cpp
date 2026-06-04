#include "chatwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

ChatWindow::ChatWindow(QWidget *parent) : QMainWindow(parent)
{
    // Clé API depuis variable d'environnement
    m_apiKey = qgetenv("MISTRAL_API_KEY");

    m_chatDisplay = new QTextEdit(this);
    m_chatDisplay->setReadOnly(true);

    m_inputField = new QLineEdit(this);
    m_inputField->setPlaceholderText("Votre message...");

    m_sendButton = new QPushButton("Envoyer", this);

    QHBoxLayout *inputRow = new QHBoxLayout();
    inputRow->addWidget(m_inputField);
    inputRow->addWidget(m_sendButton);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(m_chatDisplay);
    layout->addLayout(inputRow);

    QWidget *central = new QWidget(this);
    central->setLayout(layout);
    setCentralWidget(central);
    resize(600, 500);
    setWindowTitle("Chat OpenAI");

    m_net = new QNetworkAccessManager(this);
    connect(m_net, &QNetworkAccessManager::finished,
            this, &ChatWindow::onReplyFinished);
    connect(m_sendButton, &QPushButton::clicked,
            this, &ChatWindow::onSendClicked);
    connect(m_inputField, &QLineEdit::returnPressed,
            this, &ChatWindow::onSendClicked);
}

void ChatWindow::onSendClicked()
{
    QString text = m_inputField->text().trimmed();
    if (text.isEmpty()) return;

    appendMessage("Vous", text);
    m_inputField->clear();
    m_sendButton->setEnabled(false);

    QJsonObject userMsg;
    userMsg["role"]    = "user";
    userMsg["content"] = text;
    m_history.append(userMsg);

    // ── URL OpenAI ───────────────────────────────────────────────
    QNetworkRequest req(QUrl("https://api.mistral.ai/v1/chat/completions"));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // ── "Bearer" au lieu de "x-api-key" ─────────────────────────
    QString auth = "Bearer " + m_apiKey;
    req.setRawHeader("Authorization", auth.toUtf8());

    // ── Corps JSON ────────────────────────────────────────────────
    QJsonObject body;
    body["model"]     = "mistral-small-latest";  // gratuit et rapide
    body["max_tokens"] = 1024;
    body["messages"]   = m_history;

    m_net->post(req, QJsonDocument(body).toJson());
}

void ChatWindow::onReplyFinished(QNetworkReply *reply)
{
    m_sendButton->setEnabled(true);
    // int httpCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    // qDebug() << "Code HTTP:" << httpCode;
    // qDebug() << "Réponse brute:" << reply->readAll();


    if (reply->error() != QNetworkReply::NoError) {
        appendMessage("Erreur", reply->errorString());
        reply->deleteLater();
        return;
    }

    // ── Format de réponse OpenAI ─────────────────────────────────
    // { "choices": [ { "message": { "content": "..." } } ] }
    QJsonObject resp = QJsonDocument::fromJson(reply->readAll()).object();

    QString answer = resp["choices"].toArray()
            .first().toObject()
            ["message"].toObject()
            ["content"].toString();

    QJsonObject assistantMsg;
    assistantMsg["role"]    = "assistant";
    assistantMsg["content"] = answer;
    m_history.append(assistantMsg);

    appendMessage("Mistral", answer);
    reply->deleteLater();
}

void ChatWindow::appendMessage(const QString &role, const QString &text)
{
    m_chatDisplay->append("<b>" + role + "</b> : " + text + "\n");
}
