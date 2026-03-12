#include "interfacepcclient.h"
#include <QApplication>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), currentEditEquipIndex(-1), channelCounter(0) {

    // L'univers par défaut a été supprimé comme demandé.

    setupUi();
    setupStyle();
    resize(1100, 750);
}

MainWindow::~MainWindow() {}

void MainWindow::setupUi() {
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
    connect(btnAddMain, &QPushButton::clicked, this, &MainWindow::showAddForm);

    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();
    headerLayout->addWidget(btnAddMain);
    mainLayout->addWidget(header);

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

    refreshUniversList();
    refreshEquipmentsGrid();
}

void MainWindow::setupStyle() {
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
void MainWindow::refreshUniversList() {
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

void MainWindow::onUniversSelectionChanged() {
    bool hasSelection = uiUniversList->currentRow() >= 0;
    btnEditUnivers->setEnabled(hasSelection);
    btnDeleteUnivers->setEnabled(hasSelection);
}

void MainWindow::addUnivers() {
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

void MainWindow::editUnivers() {
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

void MainWindow::deleteUnivers() {
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

void MainWindow::showAddForm() {
    if (universList.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez créer au moins un Univers DMX d'abord.");
        return;
    }
    currentEditEquipIndex = -1; // Indique qu'on est en mode "Création"
    clearForm();
    stackedWidget->setCurrentWidget(formPage);
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

void MainWindow::refreshEquipmentsGrid() {
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

void MainWindow::editEquipment(int index) {
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

void MainWindow::deleteEquipment(int index) {
    if (index >= 0 && index < equipmentsList.size()) {
        if (QMessageBox::question(this, "Confirmation", "Supprimer cet équipement et tous ses canaux ?") == QMessageBox::Yes) {

            // Appel à la BDD avec l'ID réel
            bool succes = bdd.supprimerEquipment(equipmentsList[index].idEquipement);

            if (succes) {
                refreshEquipmentsGrid(); // Rafraîchit tout depuis le SQL
            } else {
                QMessageBox::warning(this, "Erreur", "Impossible de supprimer l'équipement.");
            }
        }
    }
}
