#include "interfacepcclient.h"
#include <QApplication>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), currentEditEquipIndex(-1), channelCounter(0) {
    setupUi();
    setupStyle();
    resize(1100, 750);
    connect(&socketClient, &QTcpSocket::connected, this, &MainWindow::onQTcpSocket_connected);
    connect(&socketClient, &QTcpSocket::disconnected, this, &MainWindow::onQTcpSocket_disconnected);
}

MainWindow::~MainWindow() {}

void MainWindow::setupUi() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QFrame* header = new QFrame();
    header->setObjectName("header");
    QHBoxLayout* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(20, 15, 20, 15);

    QVBoxLayout* titleLayout = new QVBoxLayout();
    QLabel* mainTitle = new QLabel("DMX Equipment Manager");
    mainTitle->setObjectName("mainTitle");
    titleLayout->addWidget(mainTitle);
    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();

    QPushButton* btnGoToScenes = new QPushButton("Créer les Scènes");
    btnGoToScenes->setObjectName("btnGrey");
    connect(btnGoToScenes, &QPushButton::clicked, this, &MainWindow::showScenesPage);
    headerLayout->addWidget(btnGoToScenes);

    QPushButton* btnAddMain = new QPushButton("+ Ajouter Équipement");
    btnAddMain->setObjectName("btnTeal");
    connect(btnAddMain, &QPushButton::clicked, this, &MainWindow::showAddForm);
    headerLayout->addWidget(btnAddMain);

    QPushButton* btnGoToLive = new QPushButton("Live / Régie");
    btnGoToLive->setObjectName("btnTeal");
    connect(btnGoToLive, &QPushButton::clicked, this, &MainWindow::showLivePage);
    headerLayout->addWidget(btnGoToLive);
    mainLayout->addWidget(header);

    stackedWidget = new QStackedWidget();
    mainLayout->addWidget(stackedWidget);

    statusLabel = new QLabel();
    statusLabel->setStyleSheet("color: red; font-weight: bold; margin-right: 10px;");

    statusBar()->addPermanentWidget(statusLabel);

    statusBar()->showMessage("Application démarrée !!", 5000);

    listPage = new QWidget();
    QHBoxLayout* listPageLayout = new QHBoxLayout(listPage);
    listPageLayout->setContentsMargins(0, 0, 0, 0);

    QFrame* sidebar = new QFrame();
    sidebar->setObjectName("sidebar");
    sidebar->setFixedWidth(250);
    QVBoxLayout* sidebarLayout = new QVBoxLayout(sidebar);

    QLabel* uniTitle = new QLabel("UNIVERS DMX");
    uniTitle->setObjectName("tealTitle");
    uiUniversList = new QListWidget();
    uiUniversList->setObjectName("listWidget");
    connect(uiUniversList, &QListWidget::itemSelectionChanged, this, &MainWindow::onUniversSelectionChanged);

    QHBoxLayout* uniActionsLayout = new QHBoxLayout();
    btnEditUnivers = new QPushButton("Modifier");
    btnEditUnivers->setObjectName("btnGrey");
    btnEditUnivers->setEnabled(false);
    btnDeleteUnivers = new QPushButton("Supprimer");
    btnDeleteUnivers->setObjectName("btnRed");
    btnDeleteUnivers->setEnabled(false);
    uniActionsLayout->addWidget(btnEditUnivers);
    uniActionsLayout->addWidget(btnDeleteUnivers);

    connect(btnEditUnivers, &QPushButton::clicked, this, &MainWindow::editUnivers);
    connect(btnDeleteUnivers, &QPushButton::clicked, this, &MainWindow::deleteUnivers);

    QPushButton* btnAddUnivers = new QPushButton("+ Ajouter Univers");
    btnAddUnivers->setObjectName("btnDashed");
    connect(btnAddUnivers, &QPushButton::clicked, this, &MainWindow::addUnivers);

    sidebarLayout->addWidget(uniTitle);
    sidebarLayout->addWidget(uiUniversList);
    sidebarLayout->addLayout(uniActionsLayout);
    sidebarLayout->addWidget(btnAddUnivers);
    listPageLayout->addWidget(sidebar);

    QScrollArea* listScroll = new QScrollArea();
    listScroll->setWidgetResizable(true);
    listScroll->setFrameShape(QFrame::NoFrame);
    equipmentsContainer = new QWidget();
    equipmentsContainer->setObjectName("scrollContent");
    equipmentsGrid = new QGridLayout(equipmentsContainer);
    equipmentsGrid->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    equipmentsGrid->setSpacing(15);
    listScroll->setWidget(equipmentsContainer);
    listPageLayout->addWidget(listScroll);
    stackedWidget->addWidget(listPage);

    formPage = new QWidget();
    QVBoxLayout* formPageLayout = new QVBoxLayout(formPage);
    formPageLayout->setContentsMargins(20, 20, 20, 20);

    QScrollArea* formScroll = new QScrollArea();
    formScroll->setWidgetResizable(true);
    formScroll->setFrameShape(QFrame::NoFrame);
    QWidget* formScrollContent = new QWidget();
    formScrollContent->setObjectName("scrollContent");
    QVBoxLayout* formLayout = new QVBoxLayout(formScrollContent);
    formLayout->setAlignment(Qt::AlignTop);

    QLabel* formTitle = new QLabel("Équipement DMX");
    formTitle->setObjectName("tealTitle");
    formLayout->addWidget(formTitle);

    QGridLayout* infoGrid = new QGridLayout();
    infoGrid->addWidget(new QLabel("NOM DE L'ÉQUIPEMENT"), 0, 0);
    nameEdit = new QLineEdit();
    nameEdit->setPlaceholderText("Ex: Lyre Spot 1");
    infoGrid->addWidget(nameEdit, 1, 0);

    infoGrid->addWidget(new QLabel("UNIVERS CIBLE"), 0, 1);
    universCombo = new QComboBox();
    universCombo->setObjectName("comboBox");
    infoGrid->addWidget(universCombo, 1, 1);

    infoGrid->addWidget(new QLabel("ADRESSE DE DÉPART (1-512)"), 0, 2);
    startAddressEdit = new QLineEdit();
    startAddressEdit->setPlaceholderText("Ex: 1");
    infoGrid->addWidget(startAddressEdit, 1, 2);
    formLayout->addLayout(infoGrid);
    formLayout->addSpacing(20);

    channelCountLabel = new QLabel("CANAUX: <span style='color:#107c7c;'>0</span>");
    channelCountLabel->setObjectName("boldLabel");
    formLayout->addWidget(channelCountLabel);

    channelsFormLayout = new QVBoxLayout();
    formLayout->addLayout(channelsFormLayout);

    QPushButton* btnAddChannelGlobal = new QPushButton("+ Ajouter un canal");
    btnAddChannelGlobal->setObjectName("btnDashed");
    connect(btnAddChannelGlobal, &QPushButton::clicked, [this]() { addChannelToForm(nullptr); });
    formLayout->addWidget(btnAddChannelGlobal);

    QHBoxLayout* bottomLayout = new QHBoxLayout();
    bottomLayout->addStretch();
    QPushButton* btnCancel = new QPushButton("Annuler");
    btnCancel->setObjectName("btnGrey");
    connect(btnCancel, &QPushButton::clicked, this, &MainWindow::showList);
    QPushButton* btnSave = new QPushButton("Enregistrer");
    btnSave->setObjectName("btnGreen");
    connect(btnSave, &QPushButton::clicked, this, &MainWindow::saveEquipment);

    bottomLayout->addWidget(btnCancel);
    bottomLayout->addWidget(btnSave);
    formLayout->addLayout(bottomLayout);

    formScroll->setWidget(formScrollContent);
    formPageLayout->addWidget(formScroll);
    stackedWidget->addWidget(formPage);

    scenesPage = new QWidget();
    QVBoxLayout* scenesPageLayout = new QVBoxLayout(scenesPage);
    scenesPageLayout->setContentsMargins(20, 20, 20, 20);

    QVBoxLayout* scenesTopLayout = new QVBoxLayout();
    QHBoxLayout* scenesHeaderLayout1 = new QHBoxLayout();
    QLabel* scenesTitle = new QLabel("CRÉATION & GESTION DE SCÈNES");
    scenesTitle->setObjectName("tealTitle");
    scenesUniversCombo = new QComboBox();
    scenesUniversCombo->setObjectName("comboBox");
    connect(scenesUniversCombo, &QComboBox::currentIndexChanged, this, &MainWindow::onScenesUniversChanged);
    btnResetSliders = new QPushButton("Reset 0");
    btnResetSliders->setObjectName("btnGrey");
    connect(btnResetSliders, &QPushButton::clicked, this, &MainWindow::resetSliders);
    scenesHeaderLayout1->addWidget(scenesTitle);
    scenesHeaderLayout1->addStretch();
    scenesHeaderLayout1->addWidget(new QLabel(" Univers:"));
    scenesHeaderLayout1->addWidget(scenesUniversCombo);
    scenesHeaderLayout1->addWidget(btnResetSliders);

    QHBoxLayout* scenesHeaderLayout2 = new QHBoxLayout();
    scenesCombo = new QComboBox();
    scenesCombo->setObjectName("comboBox");
    connect(scenesCombo, &QComboBox::currentIndexChanged, this, &MainWindow::onSceneSelectionChanged);
    btnRenameScene = new QPushButton("Renommer");
    btnRenameScene->setObjectName("btnGrey");
    btnRenameScene->setEnabled(false);
    connect(btnRenameScene, &QPushButton::clicked, this, &MainWindow::onRenameSceneClicked);
    btnDeleteScene = new QPushButton("Supprimer");
    btnDeleteScene->setObjectName("btnRed");
    btnDeleteScene->setEnabled(false);
    connect(btnDeleteScene, &QPushButton::clicked, this, &MainWindow::onDeleteSceneClicked);
    QPushButton* btnSaveScene = new QPushButton("Enregistrer la Scène");
    btnSaveScene->setObjectName("btnGreen");
    connect(btnSaveScene, &QPushButton::clicked, this, &MainWindow::saveCurrentScene);
    scenesHeaderLayout2->addWidget(new QLabel("Scène active:"));
    scenesHeaderLayout2->addWidget(scenesCombo, 1);
    scenesHeaderLayout2->addWidget(btnRenameScene);
    scenesHeaderLayout2->addWidget(btnDeleteScene);
    scenesHeaderLayout2->addWidget(btnSaveScene);

    scenesTopLayout->addLayout(scenesHeaderLayout1);
    scenesTopLayout->addLayout(scenesHeaderLayout2);
    scenesPageLayout->addLayout(scenesTopLayout);

    QScrollArea* slidersScroll = new QScrollArea();
    slidersScroll->setWidgetResizable(true);
    slidersScroll->setFrameShape(QFrame::NoFrame);
    QWidget* slidersContainer = new QWidget();
    QGridLayout* slidersGrid = new QGridLayout(slidersContainer);
    slidersGrid->setSpacing(10);

    for (int i = 0; i < 512; ++i) {
        int dmxChannel = i + 1;
        QFrame* sliderFrame = new QFrame();
        sliderFrame->setObjectName("card");
        sliderFrame->setFixedSize(100, 240);
        QVBoxLayout* sLayout = new QVBoxLayout(sliderFrame);
        sLayout->setAlignment(Qt::AlignHCenter);

        QLabel* lTitle = new QLabel(QString::number(dmxChannel));
        lTitle->setAlignment(Qt::AlignCenter);
        lTitle->setWordWrap(true);
        lTitle->setStyleSheet("font-size: 10px; color: #aaa;");

        QSlider* slider = new QSlider(Qt::Vertical);
        slider->setRange(0, 255);
        slider->setValue(0);

        QLabel* lVal = new QLabel("0");
        lVal->setAlignment(Qt::AlignCenter);
        lVal->setStyleSheet("font-weight: bold; color: #107c7c;");

        connect(slider, &QSlider::valueChanged, [this, i, dmxChannel, lVal, lTitle](int val){
            lVal->setText(QString::number(val));
            if (dmxSliders[i].idCanalDB != -1) {
                QString textToDisplay = dmxSliders[i].descriptionBase;
                for (const auto& f : dmxSliders[i].fonctions) {
                    if (val >= f.min && val <= f.max) {
                        textToDisplay = f.nom;
                        break;
                    }
                }
                QString titre = QString("<b>%1</b><br>%2<br><i style='color:#00e5ff;'>%3</i>")
                        .arg(dmxChannel)
                        .arg(dmxSliders[i].nomEquipement)
                        .arg(textToDisplay);
                lTitle->setText(titre);
            }
        });

        sLayout->addWidget(lTitle);
        sLayout->addWidget(slider, 1, Qt::AlignHCenter);
        sLayout->addWidget(lVal);
        slidersGrid->addWidget(sliderFrame, i / 16, i % 16);
        dmxSliders.append({-1, lTitle, slider, lVal, "", "", {}});
    }

    slidersScroll->setWidget(slidersContainer);
    slidersScroll->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scenesPageLayout->addWidget(slidersScroll, 1);

    QPushButton* btnBackToEquipments = new QPushButton("Retour aux équipements");
    btnBackToEquipments->setObjectName("btnGrey");
    connect(btnBackToEquipments, &QPushButton::clicked, this, &MainWindow::showList);
    scenesPageLayout->addWidget(btnBackToEquipments);
    stackedWidget->addWidget(scenesPage);

    livePage = new QWidget();
    QVBoxLayout* liveMainLayout = new QVBoxLayout(livePage);
    liveMainLayout->setContentsMargins(20, 20, 20, 20);

    QLabel* liveTitle = new QLabel("RÉGIE LIVE : SÉLECTION ET LANCEMENT");
    liveTitle->setObjectName("tealTitle");
    liveMainLayout->addWidget(liveTitle);

    QHBoxLayout* tcpLayout = new QHBoxLayout();
    tcpLayout->addWidget(new QLabel("Serveur cible (IP):"));
    lineEditIP = new QLineEdit("192.168.1.20");
    tcpLayout->addWidget(lineEditIP);
    tcpLayout->addWidget(new QLabel("Port:"));
    spinBoxPort = new QSpinBox();
    spinBoxPort->setRange(1, 65535);
    spinBoxPort->setValue(12345);
    tcpLayout->addWidget(spinBoxPort);
    btnConnectTCP = new QPushButton("Connexion");
    btnConnectTCP->setObjectName("btnGrey");
    connect(btnConnectTCP, &QPushButton::clicked, this, &MainWindow::on_btnConnect_clicked);
    tcpLayout->addWidget(btnConnectTCP);
    liveMainLayout->addLayout(tcpLayout);

    liveScenesList = new QListWidget();
    liveScenesList->setObjectName("listWidget");
    liveScenesList->setStyleSheet("QListWidget::item { padding: 15px; font-size: 14px; border-bottom: 1px solid #444; } "
                                  "QListWidget::item:selected { background-color: #107c7c; color: white; font-weight: bold; }");
    connect(liveScenesList, &QListWidget::itemSelectionChanged, this, &MainWindow::onLiveSceneSelected);

    btnLaunchLiveScene = new QPushButton("SÉLECTIONNEZ UNE SCÈNE");
    btnLaunchLiveScene->setObjectName("btnGreen");
    btnLaunchLiveScene->setMinimumHeight(70);
    btnLaunchLiveScene->setEnabled(false);
    QFont fontGO = btnLaunchLiveScene->font();
    fontGO.setPointSize(16);
    fontGO.setBold(true);
    btnLaunchLiveScene->setFont(fontGO);
    connect(btnLaunchLiveScene, &QPushButton::clicked, this, &MainWindow::onLaunchButtonClicked);

    liveMainLayout->addWidget(liveScenesList, 1);
    liveMainLayout->addWidget(btnLaunchLiveScene);
    stackedWidget->addWidget(livePage);

    refreshUniversList();
    refreshEquipmentsGrid();
}

void MainWindow::setupStyle() {
    QString qss = R"(
        QMainWindow { background-color: #2b2b2b; }
        QWidget#scrollContent { background-color: transparent; }
        QFrame#header { background-color: #1e1e1e; border-bottom: 1px solid #000; }
        QFrame#sidebar { background-color: #222222; border-right: 1px solid #111; }
        QLabel { color: #cccccc; font-family: 'Segoe UI', Arial, sans-serif; }
        QLabel#mainTitle { color: #ffffff; font-size: 20px; font-weight: bold; }
        QLabel#tealTitle { color: #107c7c; font-size: 16px; font-weight: bold; margin-bottom: 10px; }
        QLabel#boldLabel { font-weight: bold; font-size: 13px; color: #aaaaaa;}
        QLineEdit, QComboBox, QListWidget {
            background-color: #383838; border: 1px solid #555;
            color: #ffffff; padding: 8px; border-radius: 4px;
        }
        QLineEdit:focus, QComboBox:focus { border: 1px solid #107c7c; }
        QListWidget::item { padding: 10px; border-bottom: 1px solid #444; }
        QListWidget::item:selected { background-color: #107c7c; color: white; }
        QPushButton { border-radius: 4px; padding: 8px 16px; font-weight: bold; }
        QPushButton#btnTeal { background-color: #107c7c; color: white; border: none; }
        QPushButton#btnTeal:hover { background-color: #149999; }
        QPushButton#btnGreen { background-color: #388e3c; color: white; border: none; }
        QPushButton#btnGreen:hover { background-color: #4caf50; }
        QPushButton#btnRed { background-color: #d32f2f; color: white; border: none; }
        QPushButton#btnRed:hover { background-color: #f44336; }
        QPushButton#btnGrey { background-color: #555555; color: white; border: none; }
        QPushButton#btnGrey:hover { background-color: #666666; }
        QPushButton:disabled { background-color: #444444; color: #777777; }
        QPushButton#btnDashed {
            background-color: transparent; color: #107c7c;
            border: 1px dashed #107c7c; padding: 12px;
        }
        QPushButton#btnDashed:hover { background-color: rgba(16, 124, 124, 0.1); }
        QFrame#card { background-color: #3c3c3c; border: 1px solid #555; border-radius: 6px; }
    )";
    setStyleSheet(qss);
}

void MainWindow::refreshUniversList() {
    uiUniversList->clear();
    universCombo->clear();
    universList = bdd.chargerUnivers();
    for (const auto& u : universList) {
        QString display = QString("Univers %1 (%2)").arg(u.numero).arg(u.ip);
        uiUniversList->addItem(display);
        universCombo->addItem(display, u.idUnivers);
    }
    btnEditUnivers->setEnabled(false);
    btnDeleteUnivers->setEnabled(false);
}

void MainWindow::onUniversSelectionChanged() {
    bool hasSelection = uiUniversList->currentRow() >= 0;
    btnEditUnivers->setEnabled(hasSelection);
    btnDeleteUnivers->setEnabled(hasSelection);
}

void MainWindow::addUnivers() {
    QDialog dialog(this);
    dialog.setWindowTitle("Nouvel Univers");
    dialog.setStyleSheet(this->styleSheet());
    dialog.setMinimumWidth(450); // Agrandissement de la boîte

    QVBoxLayout layout(&dialog);
    QFormLayout form;

    // --- CALCUL DYNAMIQUE DU PROCHAIN NUMÉRO ---
    int prochainNumero = 1;
    for (const auto& u : universList) {
        if (u.numero >= prochainNumero) {
            prochainNumero = u.numero + 1;
        }
    }
    // -------------------------------------------

    QLineEdit* numEdit = new QLineEdit(&dialog);
    numEdit->setText(QString::number(prochainNumero)); // Pré-remplissage dynamique

    QLineEdit* ipEdit = new QLineEdit(&dialog);
    // Ta Regex de validation stricte
    QRegularExpression rx("^192\\.168\\.1\\.([1-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-4])$");
    QValidator *ipValidator = new QRegularExpressionValidator(rx, this);
    ipEdit->setValidator(ipValidator);
    ipEdit->setPlaceholderText("192.168.1.X");

    form.addRow("Numéro de l'univers:", numEdit);
    form.addRow("Adresse IP:", ipEdit);
    layout.addLayout(&form);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    layout.addWidget(&buttonBox);
    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted && !numEdit->text().isEmpty()) {
        if (bdd.enregistrerUnivers(numEdit->text().toInt(), ipEdit->text())) {
            refreshUniversList();
        } else {
            statusLabel->setText("Impossible d'enregistrer l'univers");
            statusLabel->setStyleSheet("color: red; font-weight: bold; margin-right: 10px;");
        }
    }
}

void MainWindow::editUnivers() {
    int row = uiUniversList->currentRow();
    if (row >= 0 && row < universList.size()) {
        UniversData& u = universList[row];
        QDialog dialog(this);
        dialog.setWindowTitle("Modifier l'Univers");
        dialog.setStyleSheet(this->styleSheet());
        dialog.setMinimumWidth(400);
        QVBoxLayout layout(&dialog);
        QFormLayout form;
        QLineEdit* numEdit = new QLineEdit(&dialog);
        numEdit->setText(QString::number(u.numero));
        QLineEdit* ipEdit = new QLineEdit(&dialog);
        ipEdit->setText(u.ip);

        // Vérifier l 'adresse ip avec une expression régulière REGEX
        QRegularExpression rx("^192\\.168\\.1\\.([1-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-4])$");
        QValidator *ipValidator = new QRegularExpressionValidator(rx, this);
        ipEdit->setValidator(ipValidator);
        ipEdit->setPlaceholderText("192.168.1.X");

        form.addRow("Numéro de l'univers:", numEdit);
        form.addRow("Adresse IP:", ipEdit);
        layout.addLayout(&form);
        QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
        layout.addWidget(&buttonBox);
        connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
        connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

        if (dialog.exec() == QDialog::Accepted && !numEdit->text().isEmpty()) {
            if (bdd.modifierUnivers(u.idUnivers, numEdit->text().toInt(), ipEdit->text()))
                refreshUniversList();
            else
                statusLabel->setText("Impossible de modifier l'univers");
            statusLabel->setStyleSheet("color: red; font-weight: bold; margin-right: 10px;");
        }
    }
}

void MainWindow::deleteUnivers() {
    int row = uiUniversList->currentRow();
    if (row >= 0 && row < universList.size()) {
        int idASupprimer = universList[row].idUnivers;

        if (QMessageBox::question(this, "Confirmation", "Supprimer cet univers ? Cela supprimera les équipements associés.") == QMessageBox::Yes) {

            // 1. Supprimer l'univers sélectionné de la BDD
            if (bdd.supprimerUnivers(idASupprimer)) {

                // 2. Recharger la liste actuelle pour avoir les données fraîches
                universList = bdd.chargerUnivers();

                // 3. Réindexer dynamiquement tous les univers restants
                // On boucle sur la liste (qui est normalement triée par l'ID ou l'ancien numéro)
                for (int i = 0; i < universList.size(); ++i) {
                    int nouveauNumero = i + 1; // On veut que ça commence à 1

                    // On ne met à jour la BDD que si le numéro a changé
                    if (universList[i].numero != nouveauNumero) {
                        bdd.modifierUnivers(universList[i].idUnivers, nouveauNumero, universList[i].ip);
                    }
                }

                // 4. Rafraîchir l'interface graphique
                refreshUniversList();
                statusBar()->showMessage("Univers supprimé et liste réindexée.", 3000);
            }
        }
    }
}

void MainWindow::showAddForm() {
    if (universList.isEmpty()) {
        statusBar()->showMessage("Il n'existe pas d'univers DMX", 3000);
    } else {
        currentEditEquipIndex = -1;
        clearForm();
        stackedWidget->setCurrentWidget(formPage);
    }
}

void MainWindow::showList() {
    stackedWidget->setCurrentWidget(listPage);
}

void MainWindow::clearForm() {
    nameEdit->clear();
    startAddressEdit->clear();
    channelCounter = 0;
    channelCountLabel->setText("CANAUX: <span style='color:#107c7c;'>0</span>");
    QLayoutItem* item;
    while ((item = channelsFormLayout->takeAt(0)) != nullptr) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }
}

void MainWindow::showScenesPage() {
    scenesUniversCombo->blockSignals(true);
    scenesUniversCombo->clear();
    for (const auto& u : universList)
        scenesUniversCombo->addItem(QString("Univers %1 (%2)").arg(u.numero).arg(u.ip), u.idUnivers);
    scenesUniversCombo->blockSignals(false);
    if (scenesUniversCombo->count() > 0) onScenesUniversChanged();
    refreshScenesList();
    stackedWidget->setCurrentWidget(scenesPage);
}

void MainWindow::onScenesUniversChanged() {
    int index = scenesUniversCombo->currentIndex();
    int idUnivers = scenesUniversCombo->currentData().toInt();
    QMap<int, DmxChannelInfo> mapCanaux = bdd.chargerMapUnivers(idUnivers);

    for (int i = 0; i < 512; ++i) {
        int dmxChannel = i + 1;
        dmxSliders[i].idCanalDB = -1;
        dmxSliders[i].nomEquipement = "";
        dmxSliders[i].descriptionBase = "";
        dmxSliders[i].fonctions.clear();

        if (mapCanaux.contains(dmxChannel)) {
            const DmxChannelInfo& info = mapCanaux[dmxChannel];
            dmxSliders[i].idCanalDB = info.idCanal;
            dmxSliders[i].nomEquipement = info.nomEquipement;
            dmxSliders[i].descriptionBase = info.description;
            dmxSliders[i].fonctions = info.fonctions;
            emit dmxSliders[i].slider->valueChanged(dmxSliders[i].slider->value());
            dmxSliders[i].labelTitre->setStyleSheet("font-size: 10px; color: white;");
        } else {
            dmxSliders[i].labelTitre->setText(QString::number(dmxChannel));
            dmxSliders[i].labelTitre->setStyleSheet("font-size: 10px; color: #555;");
            dmxSliders[i].slider->setValue(0);
        }
    }
}

void MainWindow::saveCurrentScene() {
    if (scenesUniversCombo->count() == 0) {
        statusLabel->setText("Aucun univers enregistré");
        statusLabel->setStyleSheet("color: red; font-weight: bold; margin-right: 10px;");
    } else {
        bool ok;
        QString sceneName = QInputDialog::getText(this, "Nouvelle Scène",
                                                  "Entrez le nom de la scène :", QLineEdit::Normal,
                                                  "", &ok);
        if (ok || !sceneName.trimmed().isEmpty()){

            QMap<int, int> valeursAEnregistrer;
            for (int i = 0; i < 512; ++i) {
                int idCanalDB = dmxSliders[i].idCanalDB;
                int valeur = dmxSliders[i].slider->value();
                if (idCanalDB != -1 && valeur != 0)
                    valeursAEnregistrer.insert(idCanalDB, valeur);
            }

            if (valeursAEnregistrer.isEmpty()) {
                statusLabel->setText("Aucune valeur > 0 à enregistrer");
                statusLabel->setStyleSheet("color: red; font-weight: bold; margin-right: 10px;");
            } else {

                if (bdd.enregistrerScene(sceneName, valeursAEnregistrer)) {
                    statusLabel->setText("Scène '" + sceneName + "' sauvegardée !");
                    statusLabel->setStyleSheet("color: green; font-weight: bold; margin-right: 10px;");
                    refreshScenesList();
                } else {
                    statusLabel->setText("Impossible d'enregistrer la scène");
                    statusLabel->setStyleSheet("color: red; font-weight: bold; margin-right: 10px;");
                }
            }
        }
    }
}

void MainWindow::refreshScenesList() {
    scenesCombo->blockSignals(true);
    scenesCombo->clear();
    scenesCombo->addItem("-- Nouvelle Scène --", -1);
    scenesList = bdd.chargerLesScenes();
    for(const auto& s : scenesList)
        scenesCombo->addItem(s.nomScene, s.idScene);
    scenesCombo->blockSignals(false);
}

void MainWindow::onSceneSelectionChanged() {
    int idScene = scenesCombo->currentData().toInt();
    if (idScene == -1) {
        resetSliders();
        btnRenameScene->setEnabled(false);
        btnDeleteScene->setEnabled(false);
    } else {
        btnRenameScene->setEnabled(true);
        btnDeleteScene->setEnabled(true);
        QMap<int, int> valeursEnregistrees = bdd.chargerValeursScene(idScene);
        for (int i = 0; i < 512; ++i) {
            int idCanalDB = dmxSliders[i].idCanalDB;
            dmxSliders[i].slider->setValue((idCanalDB != -1 && valeursEnregistrees.contains(idCanalDB)) ?
                                               valeursEnregistrees[idCanalDB] : 0);
        }
    }
}

void MainWindow::resetSliders() {
    for (int i = 0; i < 512; ++i)
        dmxSliders[i].slider->setValue(0);
}

void MainWindow::onRenameSceneClicked() {
    int idScene = scenesCombo->currentData().toInt();
    if (idScene == -1){
        statusLabel->setText("Aucune scène sélectionnée. Veuillez sélectionner une scène déjà existante !");
        statusLabel->setStyleSheet("color: red; font-weight: bold; margin-right: 10px;");
    } else {
        bool ok;
        QString nouveauNom = QInputDialog::getText(this, "Renommer la scène",
                                                   "Nouveau nom :", QLineEdit::Normal,
                                                   scenesCombo->currentText(), &ok);
        if (ok && !nouveauNom.isEmpty() && bdd.renommerScene(idScene, nouveauNom)) {
            refreshScenesList();
            int index = scenesCombo->findData(idScene);
            if (index != -1) scenesCombo->setCurrentIndex(index);
        }
    }
}

void MainWindow::onDeleteSceneClicked() {
    int idScene = scenesCombo->currentData().toInt();
    if (idScene == -1) {
        statusLabel->setText("Aucune scène sélectionnée. Veuillez sélectionner une scène déjà existante !");
        statusLabel->setStyleSheet("color: red; font-weight: bold; margin-right: 10px;");
    } else {
        if (QMessageBox::question(this, "Confirmation", "Supprimer cette scène ?") == QMessageBox::Yes) {
            if (bdd.supprimerScene(idScene)) {
                refreshScenesList();
                resetSliders();
            } else {
                statusLabel->setText("Impossible de supprimer la scène");
                statusLabel->setStyleSheet("color: red; font-weight: bold; margin-right: 10px;");
            }
        }
    }
}

void MainWindow::showLivePage() {
    liveScenesList->clear();
    btnLaunchLiveScene->setEnabled(false);
    btnLaunchLiveScene->setText("SÉLECTIONNEZ UNE SCÈNE");
    selectedLiveSceneId = -1;
    QList<SceneData> liste = bdd.chargerLesScenes();
    for(const auto& scene : liste) {
        QListWidgetItem* item = new QListWidgetItem(scene.nomScene);
        item->setData(Qt::UserRole, scene.idScene);
        liveScenesList->addItem(item);
    }
    stackedWidget->setCurrentWidget(livePage);
}

void MainWindow::onLiveSceneSelected() {
    if (liveScenesList->selectedItems().isEmpty()) {
        btnLaunchLiveScene->setEnabled(false);
        btnLaunchLiveScene->setText("SÉLECTIONNEZ UNE SCÈNE");
        selectedLiveSceneId = -1;
    } else {
        QListWidgetItem* item = liveScenesList->selectedItems().first();
        selectedLiveSceneId = item->data(Qt::UserRole).toInt();
        btnLaunchLiveScene->setEnabled(true);
        btnLaunchLiveScene->setText("▶ LANCER : " + item->text().toUpper());
    }
}

void MainWindow::onLaunchButtonClicked() {
    if (selectedLiveSceneId != -1) lancerScene(selectedLiveSceneId);
}

void MainWindow::lancerScene(int idScene) {
    quint16 taille = 0;
    QBuffer tampon;
    char commande = 'P';
    tampon.open(QIODevice::WriteOnly);
    QDataStream out(&tampon);
    out << taille << commande << idScene;
    qDebug() << "Envoi trame :" << QChar(commande) << "Scene ID:" << idScene;
    taille = (static_cast<quint16>(tampon.size())) - sizeof(taille);
    tampon.seek(0);
    out << taille;
    socketClient.write(tampon.buffer());
}

void MainWindow::onQTcpSocket_connected() {
    btnConnectTCP->setText("Déconnexion");
    btnConnectTCP->setObjectName("btnGreen");
    btnConnectTCP->style()->polish(btnConnectTCP);

    statusLabel->setText("Réseau : Connecté");
    statusLabel->setStyleSheet("color: green; font-weight: bold; margin-right: 10px;");
    statusBar()->showMessage("Connexion au serveur établie", 3000);
}

void MainWindow::onQTcpSocket_disconnected() {
    btnConnectTCP->setText("Connexion");
    btnConnectTCP->setObjectName("btnGrey");
    btnConnectTCP->style()->polish(btnConnectTCP);

    statusLabel->setText("Réseau : Déconnecté");
    statusLabel->setStyleSheet("color: red; font-weight: bold; margin-right: 10px;");
    statusBar()->showMessage("Perte de la connexion au serveur", 3000);
}

void MainWindow::on_btnConnect_clicked() {
    if (btnConnectTCP->text() == "Connexion") {
        QString ip = lineEditIP->text();
        int port = spinBoxPort->value();
        if (ip.isEmpty() || port <= 0) {
            statusLabel->setText("Mauvaise adresse IP ou port invalide !");
            statusLabel->setStyleSheet("color: red; font-weight: bold; margin-right: 10px;");
        }
        socketClient.connectToHost(ip, port);
    } else {
        socketClient.disconnectFromHost();

    }
}

void MainWindow::addChannelToForm(const ChannelData* data) {
    channelCounter++;
    channelCountLabel->setText(QString("CANAUX: <span style='color:#107c7c;'>%1</span>").arg(channelCounter));

    QFrame* channelFrame = new QFrame();
    channelFrame->setObjectName("card");
    QVBoxLayout* cLayout = new QVBoxLayout(channelFrame);

    QHBoxLayout* headerLayout = new QHBoxLayout();
    QLabel* cTitle = new QLabel(QString("CANAL #%1").arg(channelCounter));
    cTitle->setObjectName("boldLabel");
    QLineEdit* cDesc = new QLineEdit();
    cDesc->setPlaceholderText("Description du canal");
    if (data) cDesc->setText(data->description);
    QPushButton* btnDelChannel = new QPushButton("Supprimer");
    btnDelChannel->setObjectName("btnRed");
    headerLayout->addWidget(cTitle);
    headerLayout->addWidget(cDesc, 1);
    headerLayout->addWidget(btnDelChannel);
    cLayout->addLayout(headerLayout);

    QVBoxLayout* funcsLayout = new QVBoxLayout();
    cLayout->addLayout(funcsLayout);

    QPushButton* btnAddFunc = new QPushButton("+ Ajouter fonction");
    btnAddFunc->setObjectName("btnDashed");
    cLayout->addWidget(btnAddFunc);

    connect(btnDelChannel, &QPushButton::clicked, [this, channelFrame]() {
        channelFrame->deleteLater();
        channelCounter--;
        channelCountLabel->setText(QString("CANAUX: <span style='color:#107c7c;'>%1</span>").arg(channelCounter));
    });
    connect(btnAddFunc, &QPushButton::clicked, [this, funcsLayout]() {
        addFunctionToChannel(funcsLayout, nullptr);
    });

    if (data && !data->fonctions.isEmpty()) {
        for (const auto& func : data->fonctions) addFunctionToChannel(funcsLayout, &func);
    } else {
        addFunctionToChannel(funcsLayout, nullptr);
    }
    channelsFormLayout->addWidget(channelFrame);
}

void MainWindow::addFunctionToChannel(QVBoxLayout* functionsLayout, const FunctionData* data) {
    QWidget* funcWidget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(funcWidget);
    layout->setContentsMargins(0, 5, 0, 5);

    QLineEdit* fName = new QLineEdit(); fName->setPlaceholderText("Fonction");
    QLineEdit* fMin = new QLineEdit(); fMin->setPlaceholderText("Min"); fMin->setFixedWidth(60);
    QLineEdit* fMax = new QLineEdit(); fMax->setPlaceholderText("Max"); fMax->setFixedWidth(60);

    if (data) {
        fName->setText(data->nom);
        fMin->setText(data->min);
        fMax->setText(data->max);
    }

    layout->addWidget(fName);
    layout->addWidget(fMin);
    layout->addWidget(fMax);

    QPushButton* btnDelFunc = new QPushButton("X");
    btnDelFunc->setObjectName("btnRed");
    connect(btnDelFunc, &QPushButton::clicked, funcWidget, &QWidget::deleteLater);
    layout->addWidget(btnDelFunc);

    functionsLayout->addWidget(funcWidget);
}

void MainWindow::saveEquipment() {
    EquipmentData eq;
    eq.nom = nameEdit->text().isEmpty() ? "Nouvel Équipement" : nameEdit->text();
    eq.univers = universCombo->currentText();
    eq.dmxStart = startAddressEdit->text().isEmpty() ? "1" : startAddressEdit->text();

    for (int i = 0; i < channelsFormLayout->count(); ++i) {
        QFrame* cFrame = qobject_cast<QFrame*>(channelsFormLayout->itemAt(i)->widget());
        if (!cFrame) continue;
        ChannelData cd;
        QList<QLineEdit*> lineEdits = cFrame->findChildren<QLineEdit*>();
        if (!lineEdits.isEmpty()) {
            cd.description = lineEdits[0]->text();
            for (int j = 1; j < lineEdits.size(); j += 3) {
                if (j + 2 < lineEdits.size())
                    cd.fonctions.append({lineEdits[j]->text(), lineEdits[j+1]->text(), lineEdits[j+2]->text()});
            }
        }
        eq.canaux.append(cd);
    }

    int idU = universCombo->currentData().toInt();
    bool resultat = false;

    if (currentEditEquipIndex >= 0)
        resultat = bdd.modifierEquipment(equipmentsList[currentEditEquipIndex].idEquipement, eq, idU);
    else
        resultat = bdd.enregistrerEquipment(eq, idU);

    if (resultat) {
        refreshEquipmentsGrid();
        showList();
    } else {
        statusLabel->setText("Échec de l'opération");
        statusLabel->setStyleSheet("color: red; font-weight: bold; margin-right: 10px;");
    }
}

void MainWindow::refreshEquipmentsGrid() {
    QLayoutItem* item;
    while ((item = equipmentsGrid->takeAt(0)) != nullptr) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }
    equipmentsList = bdd.chargerTousLesEquipements();
    for (int i = 0; i < equipmentsList.size(); ++i)
        equipmentsGrid->addWidget(createEquipmentCard(equipmentsList[i], i), i / 2, i % 2);
}

QFrame* MainWindow::createEquipmentCard(const EquipmentData& eq, int index) {
    QFrame* card = new QFrame();
    card->setObjectName("card");
    card->setFixedWidth(380);
    QVBoxLayout* layout = new QVBoxLayout(card);
    QLabel* title = new QLabel(eq.nom);
    title->setStyleSheet("color: white; font-size: 16px; font-weight: bold;");
    layout->addWidget(title);
    QGridLayout* info = new QGridLayout();
    info->addWidget(new QLabel("Univers:"), 0, 0);
    info->addWidget(new QLabel(eq.univers), 0, 1);
    info->addWidget(new QLabel("Adresse DMX:"), 1, 0);
    info->addWidget(new QLabel(eq.dmxStart), 1, 1);
    info->addWidget(new QLabel("Canaux:"), 2, 0);
    info->addWidget(new QLabel(QString::number(eq.canaux.size())), 2, 1);
    layout->addLayout(info);
    QHBoxLayout* btnLayout = new QHBoxLayout();
    QPushButton* btnEdit = new QPushButton("Modifier");
    btnEdit->setObjectName("btnTeal");
    connect(btnEdit, &QPushButton::clicked, [this, index]() { editEquipment(index); });
    QPushButton* btnDel = new QPushButton("Supprimer");
    btnDel->setObjectName("btnRed");
    connect(btnDel, &QPushButton::clicked, [this, index]() { deleteEquipment(index); });
    btnLayout->addStretch();
    btnLayout->addWidget(btnEdit);
    btnLayout->addWidget(btnDel);
    layout->addLayout(btnLayout);
    return card;
}

void MainWindow::editEquipment(int index) {
    if (index >= 0 && index < equipmentsList.size()) {
        currentEditEquipIndex = index;
        const EquipmentData& eq = equipmentsList[index];
        clearForm();
        nameEdit->setText(eq.nom);
        startAddressEdit->setText(eq.dmxStart);
        for (const ChannelData& cd : eq.canaux) addChannelToForm(&cd);
        stackedWidget->setCurrentWidget(formPage);
    }
}

void MainWindow::deleteEquipment(int index) {
    if (index >= 0 && index < equipmentsList.size()) {
        if (QMessageBox::question(this, "Confirmation", "Supprimer cet équipement ?") == QMessageBox::Yes) {
            if (bdd.supprimerEquipment(equipmentsList[index].idEquipement))
                refreshEquipmentsGrid();
            else
                statusLabel->setText("Impossible de supprimer l'équipement");
            statusLabel->setStyleSheet("color: red; font-weight: bold; margin-right: 10px;");
        }
    }
}
