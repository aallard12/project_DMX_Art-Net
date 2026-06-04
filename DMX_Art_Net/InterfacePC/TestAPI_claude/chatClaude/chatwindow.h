#pragma once
#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonArray>

class QTextEdit;
class QLineEdit;
class QPushButton;

class ChatWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit ChatWindow(QWidget *parent = nullptr);

private slots:
    void onSendClicked();
    void onReplyFinished(QNetworkReply *reply);

private:
    QTextEdit   *m_chatDisplay;
    QLineEdit   *m_inputField;
    QPushButton *m_sendButton;
    QNetworkAccessManager *m_net;
    QJsonArray   m_history;
    QString      m_apiKey;

    void appendMessage(const QString &role, const QString &text);
};
