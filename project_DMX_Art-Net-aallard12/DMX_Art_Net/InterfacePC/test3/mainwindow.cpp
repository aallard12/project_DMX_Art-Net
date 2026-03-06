#include "mainwindow.h"
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>

// --- CHANNEL WIDGET IMPLEMENTATION ---
ChannelWidget::ChannelWidget(QString function, int addr, int val, QWidget *parent)
    : QWidget(parent), address(addr) {

    QVBoxLayout *layout = new QVBoxLayout(this);
    setFixedWidth(65);

    QLabel *lblFunc = new QLabel(function);
    lblFunc->setAlignment(Qt::AlignCenter);
    lblFunc->setStyleSheet("font-weight: bold; color: #FFA500; font-size: 10px;");

    QLabel *lblAddr = new QLabel(QString("%1").arg(addr));
    lblAddr->setAlignment(Qt::AlignCenter);
    lblAddr->setStyleSheet("color: #888; font-size: 9px;");

    slider = new QSlider(Qt::Vertical);
    slider->setRange(0, 255);
    slider->setValue(val);
    slider->setStyleSheet("QSlider::handle:vertical { background: #666; height: 15px; }");

    spin = new QSpinBox();
    spin->setRange(0, 255);
    spin->setValue(val);
    spin->setButtonSymbols(QAbstractSpinBox::NoButtons);
    spin->setAlignment(Qt::AlignCenter);

    layout->addWidget(lblFunc);
    layout->addWidget(lblAddr);
    layout->addWidget(slider, 0, Qt::AlignCenter);
    layout->addWidget(spin);

    connect(slider, &QSlider::valueChanged, this, &ChannelWidget::onSliderMoved);
    connect(spin, QOverload<int>::of(&QSpinBox::valueChanged), this, &ChannelWidget::onSpinChanged);
}

void ChannelWidget::onSliderMoved(int val) {
    spin->blockSignals(true);
    spin->setValue(val);
    spin->blockSignals(false);
    emit valueChanged(address, val);
}

void ChannelWidget::onSpinChanged(int val) {
    slider->blockSignals(true);
    slider->setValue(val);
    slider->blockSignals(false);
    emit valueChanged(address, val);
}

// --- FIXTURE WIDGET IMPLEMENTATION ---
FixtureWidget::FixtureWidget(QString name, QWidget *parent) : QGroupBox(name, parent) {
    setStyleSheet("QGroupBox { border: 2px solid #444; margin-top: 15px; color: white; font-weight: bold; }"
                  "QGroupBox::title { subcontrol-origin: margin; left: 10px; }");
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(5, 20, 5, 5);
}

// --- MAINWINDOW IMPLEMENTATION ---
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    for(int i=0; i<513; i++) dmxBuffer[i] = 0;

    initDatabase();
    setupUI();
    loadScene(0); // Charger la première scène par défaut
}

MainWindow::~MainWindow() {
    db.close();
}

void MainWindow::initDatabase() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("dmx_project.db");

    if (!db.open()) {
        QMessageBox::critical(this, "Erreur BDD", db.lastError().text());
        return;
    }

    QSqlQuery q;
    // Table des scènes
    q.exec("CREATE TABLE IF NOT EXISTS scenes (id INTEGER PRIMARY KEY, name TEXT)");
    // Table des équipements dans une scène
    q.exec("CREATE TABLE IF NOT EXISTS scene_fixtures (id INTEGER PRIMARY KEY, scene_id INTEGER, name TEXT, start_addr INTEGER, type_id INTEGER)");
    // Table des fonctions de canaux (Library)
    q.exec("CREATE TABLE IF NOT EXISTS channel_defs (type_id INTEGER, offset INTEGER, function_name TEXT)");
    // Table des valeurs enregistrées
    q.exec("CREATE TABLE IF NOT EXISTS scene_values (scene_id INTEGER, address INTEGER, value INTEGER, PRIMARY KEY(scene_id, address))");

    // Données de test (si vide)
    q.exec("SELECT count(*) FROM scenes");
    if (q.next() && q.value(0).toInt() == 0) {
        q.exec("INSERT INTO scenes VALUES (1, 'Spectacle Ouverture')");
        q.exec("INSERT INTO channel_defs VALUES (1, 0, 'Dimmer'), (1, 1, 'Pan'), (1, 2, 'Tilt'), (1, 3, 'Strobe')");
        q.exec("INSERT INTO scene_fixtures VALUES (1, 1, 'Lyre 1', 1, 1), (2, 1, 'Lyre 2', 10, 1)");
    }
}

void MainWindow::setupUI() {
    centralWidget = new QWidget();
    setCentralWidget(centralWidget);
    setStyleSheet("background-color: #222; color: white;");
    resize(1000, 600);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Barre du haut
    QHBoxLayout *topBar = new QHBoxLayout();
    universeCombo = new QComboBox();
    universeCombo->addItems({"Univers 1", "Univers 2"});

    sceneCombo = new QComboBox();
    QSqlQuery q("SELECT id, name FROM scenes");
    while(q.next()) sceneCombo->addItem(q.value(1).toString(), q.value(0));

    sceneNameEdit = new QLineEdit();
    sceneNameEdit->setPlaceholderText("Nom de la nouvelle scène...");
    sceneNameEdit->setFixedWidth(200);

    QPushButton *btnSave = new QPushButton("💾 Sauvegarder");
    QPushButton *btnAdd = new QPushButton("＋ Ajouter Fixture");

    topBar->addWidget(new QLabel("Scène :"));
    topBar->addWidget(sceneCombo);
    topBar->addWidget(new QLabel("Univers :"));
    topBar->addWidget(universeCombo);
    topBar->addStretch();
    topBar->addWidget(btnAdd);
    topBar->addWidget(btnSave);

    // Zone de défilement pour les Fixtures
    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("border: none; background: transparent;");

    QWidget *scrollContent = new QWidget();
    fixtureLayout = new QHBoxLayout(scrollContent);
    fixtureLayout->setAlignment(Qt::AlignLeft);
    scroll->setWidget(scrollContent);

    mainLayout->addLayout(topBar);
    mainLayout->addWidget(scroll);

    connect(sceneCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::loadScene);
    connect(btnSave, &QPushButton::clicked, this, &MainWindow::saveCurrentScene);
    connect(btnAdd, &QPushButton::clicked, this, &MainWindow::addFixtureDialog);
}

void MainWindow::loadScene(int index) {
    clearInterface();
    currentSceneId = sceneCombo->itemData(index).toInt();

    // 1. Charger les valeurs DMX de la scène en mémoire
    QSqlQuery qVal;
    qVal.prepare("SELECT address, value FROM scene_values WHERE scene_id = ?");
    qVal.addBindValue(currentSceneId);
    qVal.exec();
    while(qVal.next()) dmxBuffer[qVal.value(0).toInt()] = qVal.value(1).toInt();

    // 2. Construire l'interface des équipements
    QSqlQuery qFix;
    qFix.prepare("SELECT id, name, start_addr, type_id FROM scene_fixtures WHERE scene_id = ?");
    qFix.addBindValue(currentSceneId);
    qFix.exec();

    while(qFix.next()) {
        FixtureWidget *fw = new FixtureWidget(qFix.value(1).toString());
        QHBoxLayout *fwLayout = qobject_cast<QHBoxLayout*>(fw->layout());

        int startAddr = qFix.value(2).toInt();
        int typeId = qFix.value(3).toInt();

        // Chercher les fonctions des canaux pour ce type
        QSqlQuery qChan;
        qChan.prepare("SELECT offset, function_name FROM channel_defs WHERE type_id = ?");
        qChan.addBindValue(typeId);
        qChan.exec();

        while(qChan.next()) {
            int addr = startAddr + qChan.value(0).toInt();
            ChannelWidget *cw = new ChannelWidget(qChan.value(1).toString(), addr, dmxBuffer[addr]);
            connect(cw, &ChannelWidget::valueChanged, this, &MainWindow::onChannelChanged);
            fwLayout->addWidget(cw);
        }
        fixtureLayout->addWidget(fw);
    }
}

void MainWindow::onChannelChanged(int addr, int val) {
    if (addr >= 1 && addr <= 512) dmxBuffer[addr] = (unsigned char)val;
}

void MainWindow::saveCurrentScene() {
    QString newName = sceneNameEdit->text();

    if (newName.isEmpty()) {
        QMessageBox::warning(this, "Attention", "Veuillez saisir un nom pour la scène avant de sauvegarder.");
        return;
    }

    db.transaction();
    QSqlQuery q;

    // 1. Créer la nouvelle scène dans la table SCENES
    q.prepare("INSERT INTO scenes (name) VALUES (?)");
    q.addBindValue(newName);
    if (!q.exec()) {
        db.rollback();
        qDebug() << "Erreur création scène :" << q.lastError().text();
        return;
    }

    // Récupérer l'ID de la scène qu'on vient de créer
    int newSceneId = q.lastInsertId().toInt();

    // 2. Enregistrer les valeurs DMX dans la table scene_values
    for(int i=1; i<=512; i++) {
        if(dmxBuffer[i] > 0) {
            q.prepare("INSERT INTO scene_values (scene_id, address, value) VALUES (?, ?, ?)");
            q.addBindValue(newSceneId);
            q.addBindValue(i);
            q.addBindValue(dmxBuffer[i]);
            q.exec();
        }
    }

    if (db.commit()) {
        QMessageBox::information(this, "Succès", QString("Scène '%1' enregistrée !").arg(newName));

        // Optionnel : Mettre à jour la liste des scènes dans le ComboBox
        sceneCombo->addItem(newName, newSceneId);
        sceneNameEdit->clear();
    } else {
        db.rollback();
        QMessageBox::critical(this, "Erreur", "Impossible de valider la transaction.");
    }
}

void MainWindow::addFixtureDialog() {
    // Ici tu pourrais ouvrir un QDialog pour choisir dans la BDD
    qDebug() << "Ouverture du dialogue d'ajout...";
}

void MainWindow::clearInterface() {
    QLayoutItem *item;
    while ((item = fixtureLayout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }
}
