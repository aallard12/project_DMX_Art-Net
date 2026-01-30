#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    this->setWindowTitle("Console Art-Net - Gestion Sécurisée");
    this->resize(950, 750);

    socketClient = new QTcpSocket(this);
    initialiserIHM();

    connect(socketClient, &QTcpSocket::readyRead, this, &MainWindow::gestionReception);
}

void MainWindow::initialiserIHM() {
    onglets = new QTabWidget(this);

    // --- UC1 : RÉSEAU ---
    QWidget *tab1 = new QWidget();
    QFormLayout *fLayout = new QFormLayout(tab1);
    editIP = new QLineEdit("192.168.1.10");
    QPushButton *btnConnect = new QPushButton("Connexion au RPI");
    fLayout->addRow("IP Serveur (RPi) :", editIP);
    fLayout->addRow(btnConnect);
    connect(btnConnect, &QPushButton::clicked, this, &MainWindow::cas_configurerReseau);

    // --- UC2 : COMPOSITION (Sliders 1-255) ---
    QWidget *tab2 = new QWidget();
    QVBoxLayout *vLayout2 = new QVBoxLayout(tab2);
    editNomScene = new QLineEdit();
    editNomScene->setPlaceholderText("Nom de la scène...");

    QGroupBox *boxDMX = new QGroupBox("Contrôle des canaux");
    QVBoxLayout *vBoxSliders = new QVBoxLayout(boxDMX);

    for(int i=1; i<=8; ++i) { // 8 Sliders pour correspondre à la logique JSON
        QHBoxLayout *hRow = new QHBoxLayout();
        QLabel *name = new QLabel(QString("Canal %1 :").arg(i));
        QSlider *s = new QSlider(Qt::Horizontal);
        s->setRange(0, 255); // "0 et 256 exclus"
        s->setObjectName(QString("slider_%1").arg(i));

        QLabel *valLabel = new QLabel("0");
        valLabel->setFixedWidth(30);
        valLabel->setStyleSheet("font-weight: bold; color: #e67e22;");

        connect(s, &QSlider::valueChanged, [valLabel](int v){
            valLabel->setText(QString::number(v));
        });

        hRow->addWidget(name);
        hRow->addWidget(s);
        hRow->addWidget(valLabel);
        vBoxSliders->addLayout(hRow);
    }

    QPushButton *btnReset = new QPushButton("Réinitialiser les canaux");
    btnReset->setStyleSheet("background-color: #e74c3c; color: white; font: bold;");
    connect(btnReset, &QPushButton::clicked, this, &MainWindow::réinitialiserSliders);

    QPushButton *btnSave = new QPushButton("Enregistrer la scène (JSON)");
    btnSave->setStyleSheet("background-color: #00648c; color: white; font: bold;");

    vLayout2->addWidget(editNomScene);
    vLayout2->addWidget(boxDMX);
    vLayout2->addWidget(btnReset);
    vLayout2->addWidget(btnSave);
    connect(btnSave, &QPushButton::clicked, this, &MainWindow::cas_composerScene);

    // --- UC3 & UC4 : LIVE (Combo + Confirmation) ---
    QWidget *tab3 = new QWidget();
    QVBoxLayout *vLayout3 = new QVBoxLayout(tab3);

    QGroupBox *boxSelect = new QGroupBox("Sélection et Sécurité");
    QFormLayout *formSelect = new QFormLayout(boxSelect);

    comboScenes = new QComboBox();
    btnConfirmer = new QPushButton("Confirmer la sélection");
    btnConfirmer->setStyleSheet("background-color: #f1c40f; color: black; font-weight: bold;");

    formSelect->addRow("Choisir une scène :", comboScenes);
    formSelect->addRow(btnConfirmer);

    QPushButton *btnRefresh = new QPushButton("Actualiser la liste (UC4)");
    btnPlay = new QPushButton("LANCER LA SCÈNE (UC3)");
    btnPlay->setEnabled(false); // Désactivé par défaut
    btnPlay->setMinimumHeight(60);
    btnPlay->setStyleSheet("background-color: gray; color: white; font-weight: bold; font-size: 16px;");

    vLayout3->addWidget(btnRefresh);
    vLayout3->addWidget(boxSelect);
    vLayout3->addStretch();
    vLayout3->addWidget(btnPlay);

    connect(btnRefresh, &QPushButton::clicked, this, &MainWindow::cas_obtenirListeScenes);
    connect(btnConfirmer, &QPushButton::clicked, this, &MainWindow::cas_confirmerSelection);
    connect(btnPlay, &QPushButton::clicked, this, &MainWindow::cas_lancerScene);

    onglets->addTab(tab1, "1. Réseau");
    onglets->addTab(tab2, "2. Composition");
    onglets->addTab(tab3, "3. Live");
    setCentralWidget(onglets);
}

// --- LOGIQUE MÉTIER ---

void MainWindow::cas_configurerReseau() {
    socketClient->connectToHost(editIP->text(), 8888);
    if(socketClient->waitForConnected(2000)) {
        statusBar()->showMessage("Serveur Art-Net connecté", 5000);
    }
}

void MainWindow::cas_composerScene() {
    QJsonObject obj;
    obj["commande"] = "SAVE_SCENE";
    obj["nom"] = editNomScene->text();
    QJsonArray dmx;
    for(int i=1; i<=8; ++i) {
        QSlider *s = findChild<QSlider*>(QString("slider_%1").arg(i));
        if(s) dmx.append(s->value());
    }
    obj["valeurs"] = dmx;
    socketClient->write(QJsonDocument(obj).toJson());
    statusBar()->showMessage("Scène enregistrée avec succès", 3000);
}

void MainWindow::cas_obtenirListeScenes() {
    socketClient->write("{\"commande\":\"GET_LIST\"}");
}

void MainWindow::cas_confirmerSelection() {
    if(comboScenes->currentText().isEmpty()) return;

    sceneValidee = comboScenes->currentText();
    btnPlay->setEnabled(true);
    btnPlay->setStyleSheet("background-color: #03822C; color: white; font-weight: bold; font-size: 16px;");
    statusBar()->showMessage("Sélection validée : " + sceneValidee + ". Prêt à lancer.", 5000);
}

void MainWindow::cas_lancerScene() {
    if(sceneValidee.isEmpty()) return;

    QJsonObject obj;
    obj["commande"] = "PLAY_SCENE";
    obj["nom"] = sceneValidee;
    socketClient->write(QJsonDocument(obj).toJson());

    // Sécurité : on redésactive après le lancement
    btnPlay->setEnabled(false);
    btnPlay->setStyleSheet("background-color: gray; color: white;");
    statusBar()->showMessage("Scène " + sceneValidee + " lancée !", 3000);
}

void MainWindow::réinitialiserSliders() {
    for(int i=1; i<=8; ++i) {
        QSlider *s = findChild<QSlider*>(QString("slider_%1").arg(i));
        if(s) s->setValue(0);
    }
    statusBar()->showMessage("Canaux réinitialisés à 1", 2000);
}

void MainWindow::gestionReception() {
    QByteArray data = socketClient->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if(doc.isObject() && doc.object().contains("liste")) {
        comboScenes->clear();
        QJsonArray liste = doc.object()["liste"].toArray();
        for(auto v : liste) comboScenes->addItem(v.toString());
        btnPlay->setEnabled(false); // Reset sécurité si la liste change
        btnPlay->setStyleSheet("background-color: gray; color: white;");
    }
}

MainWindow::~MainWindow() {}
