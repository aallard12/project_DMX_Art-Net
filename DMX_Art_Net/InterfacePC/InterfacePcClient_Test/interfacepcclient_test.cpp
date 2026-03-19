#include "interfacepcclient_test.h"
#include <QApplication>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QMessageBox>

interfacepcclient_test::interfacepcclient_test(QWidget *parent)
    : QMainWindow(parent), currentEditEquipIndex(-1), channelCounter(0) {

    setupUi();
    setupStyle();
    resize(1100, 750);
    // Connexion des signaux TCP (Inspiré de dab_window)
    connect(&socketClient, &QTcpSocket::connected, this, &interfacepcclient_test::onQTcpSocket_connected);
    connect(&socketClient, &QTcpSocket::disconnected, this, &interfacepcclient_test::onQTcpSocket_disconnected);
}

interfacepcclient_test::~interfacepcclient_test() {}

void interfacepcclient_test::setupUi() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // --- HEADER ---
    QFrame* header = new QFrame();
    header->setObjectName("header");
    QHBoxLayout* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(20, 15, 20, 15);

    QVBoxLayout* titleLayout = new QVBoxLayout();
    QLabel* mainTitle = new QLabel("DMX Equipment Manager");
    mainTitle->setObjectName("mainTitle");
    titleLayout->addWidget(mainTitle);

    QPushButton* btnAddMain = new QPushButton("+ Ajouter Équipement");
    btnAddMain->setObjectName("btnTeal");
    connect(btnAddMain, &QPushButton::clicked, this, &interfacepcclient_test::showAddForm);

    QPushButton* btnGoToScenes = new QPushButton("Créer les Scènes");
    btnGoToScenes->setObjectName("btnGrey"); // Un style différent pour le distinguer
    connect(btnGoToScenes, &QPushButton::clicked, this, &interfacepcclient_test::showScenesPage);

    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();
    headerLayout->addWidget(btnGoToScenes);
    headerLayout->addWidget(btnAddMain);
    mainLayout->addWidget(header);

    QPushButton* btnGoToLive = new QPushButton("Live / Régie");
    btnGoToLive->setObjectName("btnTeal");
    connect(btnGoToLive, &QPushButton::clicked, this, &interfacepcclient_test::showLivePage);
    headerLayout->addWidget(btnGoToLive);

    // --- STACKED WIDGET ---
    stackedWidget = new QStackedWidget();
    mainLayout->addWidget(stackedWidget);

    // --- PAGE 1: LIST VIEW ---
    listPage = new QWidget();
    QHBoxLayout* listPageLayout = new QHBoxLayout(listPage);
    listPageLayout->setContentsMargins(0, 0, 0, 0);

    // Sidebar: Univers
    QFrame* sidebar = new QFrame();
    sidebar->setObjectName("sidebar");
    sidebar->setFixedWidth(250);
    QVBoxLayout* sidebarLayout = new QVBoxLayout(sidebar);

    QLabel* uniTitle = new QLabel("UNIVERS DMX");
    uniTitle->setObjectName("tealTitle");
    uiUniversList = new QListWidget();
    uiUniversList->setObjectName("listWidget");
    connect(uiUniversList, &QListWidget::itemSelectionChanged, this, &interfacepcclient_test::onUniversSelectionChanged);

    QHBoxLayout* uniActionsLayout = new QHBoxLayout();
    btnEditUnivers = new QPushButton("Modifier");
    btnEditUnivers->setObjectName("btnGrey");
    btnEditUnivers->setEnabled(false);

    btnDeleteUnivers = new QPushButton("Supprimer");
    btnDeleteUnivers->setObjectName("btnRed");
    btnDeleteUnivers->setEnabled(false);

    uniActionsLayout->addWidget(btnEditUnivers);
    uniActionsLayout->addWidget(btnDeleteUnivers);

    connect(btnEditUnivers, &QPushButton::clicked, this, &interfacepcclient_test::editUnivers);
    connect(btnDeleteUnivers, &QPushButton::clicked, this, &interfacepcclient_test::deleteUnivers);

    QPushButton* btnAddUnivers = new QPushButton("+ Ajouter Univers");
    btnAddUnivers->setObjectName("btnDashed");
    connect(btnAddUnivers, &QPushButton::clicked, this, &interfacepcclient_test::addUnivers);

    sidebarLayout->addWidget(uniTitle);
    sidebarLayout->addWidget(uiUniversList);
    sidebarLayout->addLayout(uniActionsLayout);
    sidebarLayout->addWidget(btnAddUnivers);
    listPageLayout->addWidget(sidebar);

    // Main Area: Equipments
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

    // --- PAGE 2: FORM PAGE ---
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

    // Info Equipement
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

    // --- PAGE 3: SCENES PAGE ---
    scenesPage = new QWidget();
    QVBoxLayout* scenesPageLayout = new QVBoxLayout(scenesPage);
    scenesPageLayout->setContentsMargins(20, 20, 20, 20);

    // --- DEBUT DU REMPLACEMENT DE L'EN-TÊTE ---
    QVBoxLayout* scenesTopLayout = new QVBoxLayout();

    // Ligne 1 : Titre, Choix Univers & Reset
    QHBoxLayout* scenesHeaderLayout1 = new QHBoxLayout();
    QLabel* scenesTitle = new QLabel("CRÉATION & GESTION DE SCÈNES"); // Déclaré UNE SEULE FOIS
    scenesTitle->setObjectName("tealTitle");

    scenesUniversCombo = new QComboBox();
    scenesUniversCombo->setObjectName("comboBox");
    connect(scenesUniversCombo, &QComboBox::currentIndexChanged, this, &interfacepcclient_test::onScenesUniversChanged);

    btnResetSliders = new QPushButton("Reset 0");
    btnResetSliders->setObjectName("btnGrey");
    connect(btnResetSliders, &QPushButton::clicked, this, &interfacepcclient_test::resetSliders);

    scenesHeaderLayout1->addWidget(scenesTitle);
    scenesHeaderLayout1->addStretch();
    scenesHeaderLayout1->addWidget(new QLabel(" Univers:"));
    scenesHeaderLayout1->addWidget(scenesUniversCombo);
    scenesHeaderLayout1->addWidget(btnResetSliders);

    // Ligne 2 : Gestion de la Scène
    QHBoxLayout* scenesHeaderLayout2 = new QHBoxLayout();

    scenesCombo = new QComboBox();
    scenesCombo->setObjectName("comboBox");
    connect(scenesCombo, &QComboBox::currentIndexChanged, this, &interfacepcclient_test::onSceneSelectionChanged);

    btnRenameScene = new QPushButton("Renommer");
    btnRenameScene->setObjectName("btnGrey");
    btnRenameScene->setEnabled(false); // Désactivé par défaut si aucune scène n'est choisie
    connect(btnRenameScene, &QPushButton::clicked, this, &interfacepcclient_test::onRenameSceneClicked);

    btnDeleteScene = new QPushButton("Supprimer");
    btnDeleteScene->setObjectName("btnRed");
    btnDeleteScene->setEnabled(false);
    connect(btnDeleteScene, &QPushButton::clicked, this, &interfacepcclient_test::onDeleteSceneClicked);

    QPushButton* btnSaveScene = new QPushButton("Enregistrer la Scène"); // Déclaré UNE SEULE FOIS
    btnSaveScene->setObjectName("btnGreen");
    connect(btnSaveScene, &QPushButton::clicked, this, &interfacepcclient_test::saveCurrentScene);

    scenesHeaderLayout2->addWidget(new QLabel("Scène active:"));
    scenesHeaderLayout2->addWidget(scenesCombo, 1);
    scenesHeaderLayout2->addWidget(btnRenameScene);
    scenesHeaderLayout2->addWidget(btnDeleteScene);
    scenesHeaderLayout2->addWidget(btnSaveScene);

    scenesTopLayout->addLayout(scenesHeaderLayout1);
    scenesTopLayout->addLayout(scenesHeaderLayout2);
    scenesPageLayout->addLayout(scenesTopLayout);
    // --- FIN DU REMPLACEMENT DE L'EN-TÊTE ---

    // Zone des 512 curseurs
    QScrollArea* slidersScroll = new QScrollArea();
    slidersScroll->setWidgetResizable(true);
    slidersScroll->setFrameShape(QFrame::NoFrame);

    QWidget* slidersContainer = new QWidget();
    QGridLayout* slidersGrid = new QGridLayout(slidersContainer);
    slidersGrid->setSpacing(10);

    // Création des 512 curseurs une seule fois
    for (int i = 0; i < 512; ++i) {
        int dmxChannel = i + 1;

        QFrame* sliderFrame = new QFrame();
        sliderFrame->setObjectName("card");
        sliderFrame->setFixedSize(100, 240); // Un peu plus grand pour le texte long
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

        // === NOUVELLE LOGIQUE DYNAMIQUE ===
        connect(slider, &QSlider::valueChanged, [this, i, dmxChannel, lVal, lTitle](int val){
            lVal->setText(QString::number(val));

            // Si le canal est assigné à un équipement (id != -1)
            if (dmxSliders[i].idCanalDB != -1) {
                // Par défaut, on affiche la description générale du canal
                QString textToDisplay = dmxSliders[i].descriptionBase;

                // On cherche si une fonctionnalité spécifique correspond à cette valeur
                for (const auto& f : dmxSliders[i].fonctions) {
                    if (val >= f.min && val <= f.max) {
                        textToDisplay = f.nom; // On a trouvé (ex: "Stroboscope lent")
                        break;
                    }
                }

                // On met à jour l'affichage avec l'équipement + la fonction en cours
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

        // On initialise la structure avec les nouveaux champs vides
        dmxSliders.append({-1, lTitle, slider, lVal, "", "", {}});
    }

    slidersScroll->setWidget(slidersContainer);

    slidersScroll->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scenesPageLayout->addWidget(slidersScroll, 1);

    stackedWidget->addWidget(scenesPage);

    // On ajoute un bouton pour revenir à l'accueil
    QPushButton* btnBackToEquipments = new QPushButton("Retour aux équipements");
    btnBackToEquipments->setObjectName("btnGrey");
    connect(btnBackToEquipments, &QPushButton::clicked, this, &interfacepcclient_test::showList);
    scenesPageLayout->addWidget(btnBackToEquipments);

    scenesPageLayout->addStretch(); // Pousse les éléments vers le haut

    // L'Étape la plus importante : on l'ajoute au gestionnaire de pages !
    stackedWidget->addWidget(scenesPage);

    // --- PAGE 4: LIVE PAGE (RÉGIE) ---
    livePage = new QWidget();
    QVBoxLayout* liveMainLayout = new QVBoxLayout(livePage);
    liveMainLayout->setContentsMargins(20, 20, 20, 20);

    QLabel* liveTitle = new QLabel("RÉGIE LIVE : SÉLECTION ET LANCEMENT");
    liveTitle->setObjectName("tealTitle");
    liveMainLayout->addWidget(liveTitle);

    // --- NOUVEAU : BANDEAU DE CONNEXION TCP ---
    QHBoxLayout* tcpLayout = new QHBoxLayout();
    tcpLayout->addWidget(new QLabel("Serveur cible (IP):"));

    lineEditIP = new QLineEdit("192.168.1.20"); // Valeur par défaut
    tcpLayout->addWidget(lineEditIP);

    tcpLayout->addWidget(new QLabel("Port:"));
    spinBoxPort = new QSpinBox();
    spinBoxPort->setRange(1, 65535);
    spinBoxPort->setValue(12345); // Port par défaut de ton camarade
    tcpLayout->addWidget(spinBoxPort);

    btnConnectTCP = new QPushButton("Connexion");
    btnConnectTCP->setObjectName("btnGrey");
    connect(btnConnectTCP, &QPushButton::clicked, this, &interfacepcclient_test::on_btnConnect_clicked);
    tcpLayout->addWidget(btnConnectTCP);

    liveMainLayout->addLayout(tcpLayout);
    // ------------------------------------------

    // 1. La liste des scènes
    liveScenesList = new QListWidget();

    // 1. La liste des scènes
    liveScenesList = new QListWidget();
    liveScenesList->setObjectName("listWidget"); // Pour lui donner un style de liste si tu en as un
    // On force des gros éléments pour que ce soit facile à cliquer en plein show
    liveScenesList->setStyleSheet("QListWidget::item { padding: 15px; font-size: 14px; border-bottom: 1px solid #444; } "
                                  "QListWidget::item:selected { background-color: #107c7c; color: white; font-weight: bold; }");
    connect(liveScenesList, &QListWidget::itemSelectionChanged, this, &interfacepcclient_test::onLiveSceneSelected);

    // 2. Le gros bouton de lancement
    btnLaunchLiveScene = new QPushButton("SÉLECTIONNEZ UNE SCÈNE");
    btnLaunchLiveScene->setObjectName("btnGreen");
    btnLaunchLiveScene->setMinimumHeight(70); // Un très gros bouton !
    btnLaunchLiveScene->setEnabled(false);    // Désactivé tant qu'on n'a rien choisi

    // On grossit la police du bouton
    QFont fontGO = btnLaunchLiveScene->font();
    fontGO.setPointSize(16);
    fontGO.setBold(true);
    btnLaunchLiveScene->setFont(fontGO);

    connect(btnLaunchLiveScene, &QPushButton::clicked, this, &interfacepcclient_test::onLaunchButtonClicked);

    liveMainLayout->addWidget(liveScenesList, 1); // Prend tout l'espace restant
    liveMainLayout->addWidget(btnLaunchLiveScene);

    stackedWidget->addWidget(livePage);

    // Canaux
    channelCountLabel = new QLabel("CANAUX: <span style='color:#107c7c;'>0</span>");
    channelCountLabel->setObjectName("boldLabel");
    formLayout->addWidget(channelCountLabel);

    channelsFormLayout = new QVBoxLayout();
    formLayout->addLayout(channelsFormLayout);

    QPushButton* btnAddChannelGlobal = new QPushButton("+ Ajouter un canal");
    btnAddChannelGlobal->setObjectName("btnDashed");
    connect(btnAddChannelGlobal, &QPushButton::clicked, this, [this]() { addChannelToForm(nullptr); });
    formLayout->addWidget(btnAddChannelGlobal);

    // Bottom Buttons
    QHBoxLayout* bottomLayout = new QHBoxLayout();
    bottomLayout->addStretch();
    QPushButton* btnCancel = new QPushButton("Annuler");
    btnCancel->setObjectName("btnGrey");
    connect(btnCancel, &QPushButton::clicked, this, &interfacepcclient_test::showList);
    QPushButton* btnSave = new QPushButton("Enregistrer");
    btnSave->setObjectName("btnGreen");
    connect(btnSave, &QPushButton::clicked, this, &interfacepcclient_test::saveEquipment);

    bottomLayout->addWidget(btnCancel);
    bottomLayout->addWidget(btnSave);
    formLayout->addLayout(bottomLayout);

    formScroll->setWidget(formScrollContent);
    formPageLayout->addWidget(formScroll);
    stackedWidget->addWidget(formPage);

    refreshUniversList();
    refreshEquipmentsGrid();
}

void interfacepcclient_test::setupStyle() {
    // Style identique
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

// --- GESTION DES UNIVERS (identique) ---
void interfacepcclient_test::refreshUniversList() {
    uiUniversList->clear();
    universCombo->clear();

    // NOUVEAU : On récupère la liste directement depuis la BDD
    universList = bdd.chargerUnivers();

    for (const auto& u : universList) {
        QString display = QString("Univers %1 (%2)").arg(u.numero).arg(u.ip);
        uiUniversList->addItem(display);
        // On attache l'ID de la BDD au combobox pour les futurs équipements
        universCombo->addItem(display, u.idUnivers);
    }
    btnEditUnivers->setEnabled(false);
    btnDeleteUnivers->setEnabled(false);
}

void interfacepcclient_test::onUniversSelectionChanged() {
    bool hasSelection = uiUniversList->currentRow() >= 0;
    btnEditUnivers->setEnabled(hasSelection);
    btnDeleteUnivers->setEnabled(hasSelection);
}

void interfacepcclient_test::addUnivers() {
    QDialog dialog(this);
    dialog.setWindowTitle("Nouvel Univers");
    dialog.setStyleSheet(this->styleSheet());
    QVBoxLayout layout(&dialog);

    QFormLayout form;
    QLineEdit* numEdit = new QLineEdit(&dialog);
    QLineEdit* ipEdit = new QLineEdit(&dialog);
    form.addRow("Numéro de l'univers:", numEdit);
    form.addRow("Adresse IP:", ipEdit);
    layout.addLayout(&form);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    layout.addWidget(&buttonBox);
    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted && !numEdit->text().isEmpty()) {
        // NOUVEAU : On enregistre dans la BDD
        bool success = bdd.enregistrerUnivers(numEdit->text().toInt(), ipEdit->text());

        if (success) {
            refreshUniversList(); // On recharge l'affichage depuis la BDD
        } else {
            QMessageBox::warning(this, "Erreur", "Impossible d'enregistrer l'univers dans la base de données.");
        }
    }
}

void interfacepcclient_test::editUnivers() {
    int row = uiUniversList->currentRow();

    // On englobe tout dans le if plutôt que de faire un "if (erreur) return;"
    if (row >= 0 && row < universList.size()) {
        UniversData& u = universList[row];

        QDialog dialog(this);
        dialog.setWindowTitle("Modifier l'Univers");
        dialog.setStyleSheet(this->styleSheet());
        QVBoxLayout layout(&dialog);

        QFormLayout form;
        QLineEdit* numEdit = new QLineEdit(&dialog);
        numEdit->setText(QString::number(u.numero));
        QLineEdit* ipEdit = new QLineEdit(&dialog);
        ipEdit->setText(u.ip);

        form.addRow("Numéro de l'univers:", numEdit);
        form.addRow("Adresse IP:", ipEdit);
        layout.addLayout(&form);

        QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
        layout.addWidget(&buttonBox);
        connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
        connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

        if (dialog.exec() == QDialog::Accepted && !numEdit->text().isEmpty()) {
            // Envoi de la requête de modification
            bool succes = bdd.modifierUnivers(u.idUnivers, numEdit->text().toInt(), ipEdit->text());

            if (succes) {
                refreshUniversList();
            } else {
                QMessageBox::warning(this, "Erreur", "Impossible de modifier cet univers en base de données.");
            }
        }
    }
}

void interfacepcclient_test::deleteUnivers() {
    int row = uiUniversList->currentRow();

    if (row >= 0 && row < universList.size()) {
        if (QMessageBox::question(this, "Confirmation", "Êtes-vous sûr de vouloir supprimer cet univers ?") == QMessageBox::Yes) {

            // Envoi de la requête de suppression avec l'ID
            bool succes = bdd.supprimerUnivers(universList[row].idUnivers);

            if (succes) {
                refreshUniversList();
            } else {
                QMessageBox::warning(this, "Erreur", "Impossible de supprimer cet univers. Vérifiez qu'aucun équipement n'y est encore rattaché.");
            }
        }
    }
}

// --- GESTION DE L'ÉQUIPEMENT ---

void interfacepcclient_test::showAddForm() {
    if (universList.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez créer au moins un Univers DMX d'abord.");
        return;
    }
    currentEditEquipIndex = -1; // Indique qu'on est en mode "Création"
    clearForm();
    stackedWidget->setCurrentWidget(formPage);
}

void interfacepcclient_test::showList() {
    stackedWidget->setCurrentWidget(listPage);
}

void interfacepcclient_test::clearForm() {
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

void interfacepcclient_test::showScenesPage()
{
    scenesUniversCombo->blockSignals(true); // Evite de déclencher le changed tout de suite
    scenesUniversCombo->clear();
    for (const auto& u : universList) {
        scenesUniversCombo->addItem(QString("Univers %1 (%2)").arg(u.numero).arg(u.ip), u.idUnivers);
    }
    scenesUniversCombo->blockSignals(false);

    if (scenesUniversCombo->count() > 0) {
        onScenesUniversChanged(); // Charge les canaux du premier univers
    }

    refreshScenesList();

    stackedWidget->setCurrentWidget(scenesPage);
}

void interfacepcclient_test::onScenesUniversChanged()
{
    int index = scenesUniversCombo->currentIndex();
    if (index < 0) return;

    int idUnivers = scenesUniversCombo->currentData().toInt();
    QMap<int, DmxChannelInfo> mapCanaux = bdd.chargerMapUnivers(idUnivers);

    for (int i = 0; i < 512; ++i) {
        int dmxChannel = i + 1;

        // Remise à zéro totale du curseur en mémoire
        dmxSliders[i].idCanalDB = -1;
        dmxSliders[i].nomEquipement = "";
        dmxSliders[i].descriptionBase = "";
        dmxSliders[i].fonctions.clear();

        if (mapCanaux.contains(dmxChannel)) {
            const DmxChannelInfo& info = mapCanaux[dmxChannel];

            // On charge les données dans le curseur
            dmxSliders[i].idCanalDB = info.idCanal;
            dmxSliders[i].nomEquipement = info.nomEquipement;
            dmxSliders[i].descriptionBase = info.description;
            dmxSliders[i].fonctions = info.fonctions;

            // On déclenche manuellement la mise à jour visuelle pour la valeur 0
            emit dmxSliders[i].slider->valueChanged(dmxSliders[i].slider->value());
            dmxSliders[i].labelTitre->setStyleSheet("font-size: 10px; color: white;");
        } else {
            // Si le canal est vide (sans équipement)
            dmxSliders[i].labelTitre->setText(QString::number(dmxChannel));
            dmxSliders[i].labelTitre->setStyleSheet("font-size: 10px; color: #555;");
            dmxSliders[i].slider->setValue(0);
        }
    }
}

void interfacepcclient_test::saveCurrentScene()
{
    if (scenesUniversCombo->count() == 0) return;

    // Demander le nom de la scène via une petite fenêtre de dialogue
    bool ok;
    QString sceneName = QInputDialog::getText(this, "Nouvelle Scène",
                                              "Entrez le nom de la scène :", QLineEdit::Normal,
                                              "", &ok);

    // Si l'utilisateur annule ou laisse vide, on arrête
    if (!ok || sceneName.trimmed().isEmpty()) {
        return;
    }

    QMap<int, int> valeursAEnregistrer;

    // Le filtrage strict !
    for (int i = 0; i < 512; ++i) {
        int idCanalDB = dmxSliders[i].idCanalDB;
        int valeur = dmxSliders[i].slider->value();

        // Règle 1 : idCanalDB != -1 (le canal existe en base / est rattaché à un équipement)
        // Règle 2 : valeur != 0 (on n'encombre pas la base avec des zéros)
        if (idCanalDB != -1 && valeur != 0) {
            valeursAEnregistrer.insert(idCanalDB, valeur);
        }
    }

    // Vérification : s'il n'y a rien à sauvegarder (tout est à 0)
    if (valeursAEnregistrer.isEmpty()) {
        QMessageBox::information(this, "Info", "Aucun canal enregistré n'a de valeur supérieure à 0.\nLa scène n'a pas été sauvegardée pour éviter d'encombrer la base de données.");
        return;
    }

    // Envoi à la base de données
    if (bdd.enregistrerScene(sceneName, valeursAEnregistrer)) {
        QMessageBox::information(this, "Succès", "La scène '" + sceneName + "' a été sauvegardée avec succès !");
        refreshScenesList();
    } else {
        QMessageBox::critical(this, "Erreur", "Impossible d'enregistrer la scène en base de données.");
    }
}

void interfacepcclient_test::refreshScenesList()
{
    scenesCombo->blockSignals(true);
    scenesCombo->clear();

    // Le choix -1 servira de repère pour savoir si on crée ou si on modifie
    scenesCombo->addItem("-- Nouvelle Scène (Vide) --", -1);

    scenesList = bdd.chargerLesScenes();
    for(const auto& s : scenesList) {
        scenesCombo->addItem(s.nomScene, s.idScene);
    }

    scenesCombo->blockSignals(false);
}

void interfacepcclient_test::onSceneSelectionChanged()
{
    int idScene = scenesCombo->currentData().toInt();

    if (idScene == -1) {
        // Mode Création : on remet tout à 0 et on désactive les boutons modif/suppr
        resetSliders();
        btnRenameScene->setEnabled(false);
        btnDeleteScene->setEnabled(false);
        return;
    }

    // Mode Édition : on active les boutons
    btnRenameScene->setEnabled(true);
    btnDeleteScene->setEnabled(true);

    // On récupère les valeurs sauvegardées pour cette scène
    QMap<int, int> valeursEnregistrees = bdd.chargerValeursScene(idScene);

    // On met à jour nos 512 curseurs
    for (int i = 0; i < 512; ++i) {
        int idCanalDB = dmxSliders[i].idCanalDB;

        // Si le curseur est lié à un canal (id != -1) ET que la scène a une valeur pour lui
        if (idCanalDB != -1 && valeursEnregistrees.contains(idCanalDB)) {
            dmxSliders[i].slider->setValue(valeursEnregistrees[idCanalDB]);
        } else {
            // Sinon, c'est qu'il était à 0 lors de l'enregistrement de la scène
            dmxSliders[i].slider->setValue(0);
        }
    }
}

void interfacepcclient_test::resetSliders()
{
    for (int i = 0; i < 512; ++i) {
        dmxSliders[i].slider->setValue(0);
    }
}

void interfacepcclient_test::onRenameSceneClicked() {
    int idScene = scenesCombo->currentData().toInt();
    QString ancienNom = scenesCombo->currentText();

    if (idScene == -1) return;

    bool ok;
    QString nouveauNom = QInputDialog::getText(this, "Renommer la scène",
                                               "Nouveau nom :", QLineEdit::Normal,
                                               ancienNom, &ok);
    if (ok && !nouveauNom.isEmpty()) {
        if (bdd.renommerScene(idScene, nouveauNom)) {
            refreshScenesList(); // Rafraîchit la combo
            // On repositionne la sélection sur la scène renommée
            int index = scenesCombo->findData(idScene);
            if (index != -1) scenesCombo->setCurrentIndex(index);
        }
    }
}

void interfacepcclient_test::onDeleteSceneClicked() {
    int idScene = scenesCombo->currentData().toInt();
    if (idScene == -1) return;

    auto result = QMessageBox::question(this, "Confirmation",
                                        "Voulez-vous vraiment supprimer cette scène ?",
                                        QMessageBox::Yes | QMessageBox::No);

    if (result == QMessageBox::Yes) {
        if (bdd.supprimerScene(idScene)) {
            refreshScenesList(); // La combo revient par défaut sur "Nouvelle Scène"
            resetSliders();
        } else {
            QMessageBox::critical(this, "Erreur", "Impossible de supprimer la scène.");
        }
    }
}

void interfacepcclient_test::showLivePage()
{
    liveScenesList->clear();
    btnLaunchLiveScene->setEnabled(false);
    btnLaunchLiveScene->setText("SÉLECTIONNEZ UNE SCÈNE");
    selectedLiveSceneId = -1;

    // On charge les scènes depuis la BDD
    QList<SceneData> liste = bdd.chargerLesScenes();

    for(const auto& scene : liste) {
        QListWidgetItem* item = new QListWidgetItem(scene.nomScene);
        // On cache l'ID de la scène dans les données de l'item (UserRole)
        item->setData(Qt::UserRole, scene.idScene);
        liveScenesList->addItem(item);
    }

    stackedWidget->setCurrentWidget(livePage);
}

void interfacepcclient_test::onLiveSceneSelected()
{
    if (liveScenesList->selectedItems().isEmpty()) {
        btnLaunchLiveScene->setEnabled(false);
        btnLaunchLiveScene->setText("SÉLECTIONNEZ UNE SCÈNE");
        selectedLiveSceneId = -1;
        return;
    }

    // On récupère l'item sélectionné
    QListWidgetItem* item = liveScenesList->selectedItems().first();

    // On met à jour notre variable avec l'ID caché
    selectedLiveSceneId = item->data(Qt::UserRole).toInt();

    // On active le bouton GO et on met à jour son texte pour confirmer l'action
    btnLaunchLiveScene->setEnabled(true);
    btnLaunchLiveScene->setText("▶ LANCER : " + item->text().toUpper());
}

void interfacepcclient_test::onLaunchButtonClicked()
{
    if (selectedLiveSceneId != -1) {
        // C'est ici qu'on appelle la vraie méthode d'envoi !
        lancerScene(selectedLiveSceneId);
    }
}

void interfacepcclient_test::lancerScene(int idScene)
{
    // On bloque l'envoi si on n'est pas connecté
    if (socketClient.state() != QAbstractSocket::ConnectedState) {
        QMessageBox::warning(this, "Erreur d'envoi", "Veuillez d'abord vous connecter au serveur TCP (bouton Connexion).");
        return;
    }

    // 1. Récupération des données en base
    QMap<int, int> valeursMap = bdd.chargerValeursScene(idScene);
    int numeroUnivers = bdd.getUniversDeScene(idScene); // NOUVEAU : on récupère l'univers

    // 2. Construire la trame brute DMX de 512 canaux
    QByteArray dmxFrame;
    dmxFrame.fill(0, 512);

    for(auto it = valeursMap.begin(); it != valeursMap.end(); ++it) {
        int indexCanal = it.key() - 1; // -1 car le DMX commence à 1
        if(indexCanal >= 0 && indexCanal < 512) {
            dmxFrame[indexCanal] = (static_cast<char>(it.value()));
        }
    }

    // --- 3. ENVOI DE LA TRAME STRICTEMENT FAÇON DAB_WINDOW ---
    quint16 taille = 0;
    quint16 universAEnvoyer = static_cast<quint16>(numeroUnivers); // On cast sur 16 bits pour être propre

    QBuffer tampon;
    tampon.open(QIODevice::WriteOnly);
    QDataStream out(&tampon);

    // Construction : on pousse la taille (vide pour l'instant), l'univers, puis les 512 octets
    out << taille << universAEnvoyer << dmxFrame;

    // Calcul de la taille réelle du paquet (taille totale moins les 2 octets du champ 'taille' lui-même)
    taille = (static_cast<quint16>(tampon.size())) - sizeof(taille);

    // Retour au début du flux pour insérer la bonne taille
    tampon.seek(0);
    out << taille;

    // Envoi effectif sur le réseau
    socketClient.write(tampon.buffer());
    // ----------------------------------------------------------

    qDebug() << "Scène" << idScene << "lancée sur l'univers" << universAEnvoyer
             << ". Trame de" << tampon.size() << "octets envoyée.";
}

void interfacepcclient_test::onQTcpSocket_connected()
{
    btnConnectTCP->setText("Déconnexion");
    btnConnectTCP->setObjectName("btnGreen");
    btnConnectTCP->style()->unpolish(btnConnectTCP);
    btnConnectTCP->style()->polish(btnConnectTCP); // Force le rafraîchissement visuel
    qDebug() << "Connecté au serveur du camarade !";
}

void interfacepcclient_test::onQTcpSocket_disconnected()
{
    btnConnectTCP->setText("Connexion");
    btnConnectTCP->setObjectName("btnGrey");
    btnConnectTCP->style()->unpolish(btnConnectTCP);
    btnConnectTCP->style()->polish(btnConnectTCP);
    qDebug() << "Déconnecté du serveur.";
}

void interfacepcclient_test::on_btnConnect_clicked()
{
    if (btnConnectTCP->text() == "Connexion"){
        socketClient.connectToHost(lineEditIP->text(), spinBoxPort->value());
    } else {
        socketClient.disconnectFromHost();
    }
}

void interfacepcclient_test::addChannelToForm(const ChannelData* data) {
    channelCounter++;
    channelCountLabel->setText(QString("CANAUX: <span style='color:#107c7c;'>%1</span>").arg(channelCounter));

    QFrame* channelFrame = new QFrame();
    channelFrame->setObjectName("card");
    QVBoxLayout* cLayout = new QVBoxLayout(channelFrame);

    QHBoxLayout* headerLayout = new QHBoxLayout();
    QLabel* cTitle = new QLabel(QString("CANAL #%1").arg(channelCounter));
    cTitle->setObjectName("boldLabel");

    QLineEdit* cDesc = new QLineEdit();
    cDesc->setPlaceholderText("Description du canal (ex: Couleur, Mouvement...)");
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

    // Si on modifie, on charge les fonctions existantes. Sinon on met 1 par défaut.
    if (data && !data->fonctions.isEmpty()) {
        for (const auto& func : data->fonctions) {
            addFunctionToChannel(funcsLayout, &func);
        }
    } else {
        addFunctionToChannel(funcsLayout, nullptr);
    }

    channelsFormLayout->addWidget(channelFrame);
}

void interfacepcclient_test::addFunctionToChannel(QVBoxLayout* functionsLayout, const FunctionData* data) {
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

void interfacepcclient_test::saveEquipment() {
    EquipmentData eq;
    eq.nom = nameEdit->text().isEmpty() ? "Nouvel Équipement" : nameEdit->text();
    eq.univers = universCombo->currentText();
    eq.dmxStart = startAddressEdit->text().isEmpty() ? "1" : startAddressEdit->text();

    // Extraction dynamique des données du formulaire
    for (int i = 0; i < channelsFormLayout->count(); ++i) {
        QFrame* cFrame = qobject_cast<QFrame*>(channelsFormLayout->itemAt(i)->widget());
        if (!cFrame) continue;

        ChannelData cd;
        // Qt trouve les LineEdits dans l'ordre de leur création/ajout au layout
        QList<QLineEdit*> lineEdits = cFrame->findChildren<QLineEdit*>();

        if (!lineEdits.isEmpty()) {
            cd.description = lineEdits[0]->text(); // 0 est cDesc

            // Les fonctions arrivent par groupe de 3 (Nom, Min, Max) à partir de l'index 1
            for (int j = 1; j < lineEdits.size(); j += 3) {
                if (j + 2 < lineEdits.size()) {
                    cd.fonctions.append({lineEdits[j]->text(), lineEdits[j+1]->text(), lineEdits[j+2]->text()});
                }
            }
        }
        eq.canaux.append(cd);
    }

    int idU = universCombo->currentData().toInt();
    bool resultat = false;

    if (currentEditEquipIndex >= 0) {
        // MODIFICATION
        int idEq = equipmentsList[currentEditEquipIndex].idEquipement;
        resultat = bdd.modifierEquipment(idEq, eq, idU);
    } else {
        // AJOUT
        resultat = bdd.enregistrerEquipment(eq, idU);
    }

    if (resultat) {
        refreshEquipmentsGrid();
        showList();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de l'opération SQL. Vérifiez les logs.");
    }
}

void interfacepcclient_test::refreshEquipmentsGrid() {
    // Nettoyer la grille actuelle
    QLayoutItem* item;
    while ((item = equipmentsGrid->takeAt(0)) != nullptr) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    // NOUVEAU : Charger depuis la BDD
    equipmentsList = bdd.chargerTousLesEquipements();

    // Reconstruire les cartes
    for (int i = 0; i < equipmentsList.size(); ++i) {
        QFrame* card = createEquipmentCard(equipmentsList[i], i);
        equipmentsGrid->addWidget(card, i / 2, i % 2);
    }
}

QFrame* interfacepcclient_test::createEquipmentCard(const EquipmentData& eq, int index) {
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

    // NOUVEAU: Bouton Modifier
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

void interfacepcclient_test::editEquipment(int index) {
    if (index >= 0 && index < equipmentsList.size()) {
        currentEditEquipIndex = index;
        const EquipmentData& eq = equipmentsList[index];

        clearForm();
        nameEdit->setText(eq.nom);
        startAddressEdit->setText(eq.dmxStart);

        // Sélectionner le bon univers dans le combo via son ID
        // On cherche l'ID qui correspond à celui stocké dans l'équipement
        // (Il faudra que chargerTousLesEquipements remplisse aussi idUnivers dans EquipmentData)

        for (const ChannelData& cd : eq.canaux) {
            addChannelToForm(&cd);
        }
        stackedWidget->setCurrentWidget(formPage);
    }
}

void interfacepcclient_test::deleteEquipment(int index) {
    if (index >= 0 && index < equipmentsList.size()) {
        // On récupère l'idEquipement stocké dans la structure lors du chargement
        int idReal = equipmentsList[index].idEquipement;

        if (QMessageBox::question(this, "Confirmation", "Supprimer cet équipement ?") == QMessageBox::Yes) {
            if (bdd.supprimerEquipment(idReal)) {
                refreshEquipmentsGrid(); // Recharge la liste depuis la BDD
            } else {
                QMessageBox::warning(this, "Erreur", "Impossible de supprimer l'équipement. "
                                                     "Vous avez créer une scène avec cet équipement. "
                                                     "Veuillez supprimer cette scène avant de supprimer l'équipement");
            }
        }
    }
}
