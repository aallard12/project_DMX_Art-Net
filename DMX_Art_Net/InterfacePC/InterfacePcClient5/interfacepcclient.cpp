/**
 * @file interfacepcclient.cpp
 * @brief Implémentation de la classe InterfacePcClient
 * @author Antoine ALLARD
 * @date 10/04/2026
 * @details Cette classe implémente la fenêtre principale de l'application cliente.
 * Elle permet la gestion des univers, des équipements et des scènes, ainsi que leur
 * visualisation et leur contrôle via des canaux DMX. Elle gère également
 * la communication réseau avec un serveur à l’aide de sockets TCP.
 */

#include "interfacepcclient.h"
#include "ui_interfacepcclient.h"

/**
 * @brief InterfacePcClient::InterfacePcClient
 * @details Constructeur. Initialise l'UI, le label de statut, les signaux TCP,
 *        puis charge la liste des univers et la grille des équipements.
 * @param parent Widget parent Qt (nullptr si fenêtre racine).
 */
InterfacePcClient::InterfacePcClient(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::InterfacePcClient)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    ui->centralWidget->showMaximized();
    statusLabel = new QLabel;
    statusLabel->setStyleSheet("color: red; font-weight: bold; margin-right: 10px;");
    ui->statusBar->addPermanentWidget(statusLabel);
    ui->statusBar->showMessage("Application démarrée !!", 5000);

    connect(&socketClient, &QTcpSocket::connected,    this, &InterfacePcClient::onQTcpSocket_connected);
    connect(&socketClient, &QTcpSocket::disconnected, this, &InterfacePcClient::onQTcpSocket_disconnected);

    refreshUniversList();
    refreshEquipmentsGrid();
}

/**
 * @brief InterfacePcClient::~InterfacePcClient
 * @details Libère l'interface graphique générée par Qt Designer.
 */
InterfacePcClient::~InterfacePcClient()
{
    delete ui;
}

/**
 * @brief InterfacePcClient::on_btnGoToList_clicked
 * @details Navigue vers la page de liste et rafraîchit les univers et les équipements.
 */
void InterfacePcClient::on_btnGoToList_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->listPage);
    refreshUniversList();
    refreshEquipmentsGrid();
}

/**
 * @brief InterfacePcClient::on_btnAddMain_clicked
 * @details Prépare le formulaire en mode création (index = -1) et navigue vers celui-ci.
 */
void InterfacePcClient::on_btnAddMain_clicked()
{
    currentEditEquipIndex = -1;
    clearForm();
    ui->stackedWidget->setCurrentWidget(ui->formPage);
}

/**
 * @brief InterfacePcClient::on_btnGoToScenes_clicked
 * @details Peuple le combo d'univers de la page Scènes, charge les sliders DMX
 *        du premier univers, rafraîchit la liste des scènes, puis navigue vers la page Scènes.
 */
void InterfacePcClient::on_btnGoToScenes_clicked()
{
    ui->scenesUniversCombo->blockSignals(true);
    ui->scenesUniversCombo->clear();
    for (const auto& u : universList)
        ui->scenesUniversCombo->addItem(QString("Univers %1 (%2)").arg(u.numero).arg(u.ip), u.idUnivers);
    ui->scenesUniversCombo->blockSignals(false);
    if (ui->scenesUniversCombo->count() > 0)
        on_scenesUniversCombo_currentIndexChanged(0);
    refreshScenesList();
    ui->stackedWidget->setCurrentWidget(ui->scenesPage);
}

/**
 * @brief InterfacePcClient::on_btnGoToLive_clicked
 * @details Recharge les univers, peuple le combo live, réinitialise la sélection
 *        de scène, charge toutes les scènes (filtre = -1), puis navigue vers la page Live.
 */
void InterfacePcClient::on_btnGoToLive_clicked()
{
    ui->liveUniversCombo->blockSignals(true);
    ui->liveUniversCombo->clear();
    ui->liveUniversCombo->addItem("Tous les univers", -1);
    universList = bdd.chargerUnivers();
    for (const auto& u : universList)
        ui->liveUniversCombo->addItem(QString("Univers %1 (%2)").arg(u.numero).arg(u.ip), u.idUnivers);
    ui->liveUniversCombo->blockSignals(false);

    // Réinitialiser la sélection
    ui->btnLaunchLiveScene->setEnabled(false);
    ui->btnLaunchLiveScene->setText("SÉLECTIONNEZ UNE SCÈNE");
    selectedLiveSceneId = -1;

    // Charger toutes les scènes (filtre = -1 = tous)
    refreshLiveScenesList(-1);

    ui->stackedWidget->setCurrentWidget(ui->livePage);
}

/**
 * @brief InterfacePcClient::on_btnAddUnivers_clicked
 * @details Ouvre le dialogue d'ajout d'univers et enregistre le résultat en base de données.
 */
void InterfacePcClient::on_btnAddUnivers_clicked()
{
    DialogUnivers univers;

    if (univers.exec() == QDialog::Accepted && univers.getNumUnivers() != 0 && !univers.getIpUnivers().isEmpty()){
        if (bdd.enregistrerUnivers(univers.getNumUnivers(), univers.getIpUnivers())){
            refreshUniversList();
            // qDebug() << "Bddok";
        }else {
            statusBar()->showMessage("Impossible d'enregistrer l'univers");
        }
    }
}

/**
 * @brief InterfacePcClient::refreshUniversList
 * @details Recharge depuis la BDD et repeuple uiUniversList, universCombo et filterUniversCombo.
 *        Désactive les boutons Modifier/Supprimer tant qu'aucun univers n'est sélectionné.
 */
void InterfacePcClient::refreshUniversList()
{
    ui->uiUniversList->clear();
    ui->universCombo->clear();
    ui->filterUniversCombo->blockSignals(true);
    ui->filterUniversCombo->clear();
    ui->filterUniversCombo->addItem("Tous les univers", -1);
    universList = bdd.chargerUnivers();
    for (const auto& u : universList) {
        QString display = QString("Univers %1 (%2)").arg(u.numero).arg(u.ip);
        ui->uiUniversList->addItem(display);
        ui->universCombo->addItem(display, u.idUnivers);
        ui->filterUniversCombo->addItem(display, u.idUnivers);
    }
    ui->filterUniversCombo->blockSignals(false);
    ui->btnEditUnivers->setEnabled(false);
    ui->btnDeleteUnivers->setEnabled(false);
}

/**
 * @brief InterfacePcClient::on_btnEditUnivers_clicked
 * @details Ouvre le dialogue de modification pour l'univers sélectionné et met à jour la BDD.
 */
void InterfacePcClient::on_btnEditUnivers_clicked()
{
    int row = ui->uiUniversList->currentRow();
    UniversData& u = universList[row];
    DialogUnivers univers;

    if (univers.exec() == QDialog::Accepted && univers.getNumUnivers() != 0 && !univers.getIpUnivers().isEmpty()){
        if (bdd.modifierUnivers(u.idUnivers, univers.getNumUnivers(), univers.getIpUnivers())){
            refreshUniversList();
            statusLabel->setText("Modification de l'univers réussie");
            statusLabel->setStyleSheet("color: green; font-weight: bold; margin-right: 10px;");
        }else {
            statusLabel->setText("Impossible de modifier l'univers");
            statusLabel->setStyleSheet("color: red; font-weight: bold; margin-right: 10px;");
        }
    }
}

/**
 * @brief InterfacePcClient::on_btnDeleteUnivers_clicked
 * @details Supprime l'univers sélectionné après confirmation, puis réindexe les numéros restants.
 */
void InterfacePcClient::on_btnDeleteUnivers_clicked()
{
    int row = ui->uiUniversList->currentRow();
    if (row < 0 || row >= universList.size()) return;

    if (QMessageBox::question(this, "Confirmation",
                              "Supprimer cet univers ? Cela supprimera les équipements associés.")
            != QMessageBox::Yes) return;

    if (bdd.supprimerUnivers(universList[row].idUnivers)) {
        universList = bdd.chargerUnivers();
        for (int i = 0; i < universList.size(); ++i) {
            if (universList[i].numero != i + 1)
                bdd.modifierUnivers(universList[i].idUnivers, i + 1, universList[i].ip);
        }
        refreshUniversList();
        statusBar()->showMessage("Univers supprimé et liste réindexée.", 3000);
    }
}

/**
 * @brief InterfacePcClient::on_uiUniversList_currentRowChanged
 * @details Active ou désactive les boutons Modifier/Supprimer selon la sélection courante.
 * @param currentRow Index de la ligne sélectionnée (-1 si aucune).
 */
void InterfacePcClient::on_uiUniversList_currentRowChanged(int currentRow)
{
    bool hasSelection = ui->uiUniversList->currentRow() >= 0;
    ui->btnEditUnivers->setEnabled(hasSelection);
    ui->btnDeleteUnivers->setEnabled(hasSelection);
}

/**
 * @brief InterfacePcClient::refreshEquipmentsGrid
 * @details Vide et recharge la grille des équipements en appliquant le filtre d'univers actif.
 *        Les équipements sont affichés sous forme de cartes sur 4 colonnes maximum.
 */
void InterfacePcClient::refreshEquipmentsGrid()
{
    ui->equipmentsGrid->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    ui->equipmentsGrid->setSpacing(15);

    QLayoutItem* item;
    while ((item = ui->equipmentsGrid->takeAt(0)) != nullptr) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }
    equipmentsList = bdd.chargerTousLesEquipements();
    int idFiltre = ui->filterUniversCombo->currentData().toInt();
    QString filtreUniversLabel = ui->filterUniversCombo->currentText(); // "Univers X ..."

    int col = 0;
    int row = 0;
    for (int i = 0; i < equipmentsList.size(); ++i) {
        // Filtrage : si -1 on affiche tout, sinon on compare le label univers
        if (idFiltre != -1 && equipmentsList[i].univers != filtreUniversLabel.left(filtreUniversLabel.indexOf(" ("))) {
            // Comparaison robuste via l'idUnivers stocké dans UniversData
            bool match = false;
            for (const auto& u : universList) {
                if (u.idUnivers == idFiltre &&
                        equipmentsList[i].univers == QString("Univers %1").arg(u.numero)) {
                    match = true;
                    break;
                }
            }
            if (!match) continue;
        }
        ui->equipmentsGrid->addWidget(createEquipmentCard(equipmentsList[i], i), row, col % 4);
        col++;
        if (col % 4 == 0) row++;
    }
}

/**
 * @brief InterfacePcClient::createEquipmentCard
 * @details Crée et retourne un QFrame stylisé affichant les informations d'un équipement
 *        (nom, univers, adresse DMX, nombre de canaux) avec les boutons Modifier et Supprimer.
 * @param eq    Données de l'équipement à afficher.
 * @param index Index dans equipmentsList, transmis aux boutons Modifier/Supprimer.
 * @return Pointeur vers le QFrame créé.
 */
QFrame *InterfacePcClient::createEquipmentCard(const EquipmentData &eq, int index)
{
    QFrame* card = new QFrame();
    card->setObjectName("card");
    card->setStyleSheet("QFrame#card { background-color: #3c3c3c; border: 1px solid #555; border-radius: 6px; }");
    QVBoxLayout* layout = new QVBoxLayout(card);

    QLabel* title = new QLabel(eq.nom);
    title->setStyleSheet("color: white; font-size: 16px; font-weight: bold;");
    layout->addWidget(title);

    QGridLayout* info = new QGridLayout();
    info->addWidget(new QLabel("Univers:"),     0, 0); info->addWidget(new QLabel(eq.univers),                        0, 1);
    info->addWidget(new QLabel("Adresse DMX:"), 1, 0); info->addWidget(new QLabel(eq.dmxStart),                       1, 1);
    info->addWidget(new QLabel("Canaux:"),      2, 0); info->addWidget(new QLabel(QString::number(eq.canaux.size())), 2, 1);
    layout->addLayout(info);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    QPushButton* btnEdit = new QPushButton("Modifier");
    btnEdit->setStyleSheet("QPushButton { background-color: #555555; color: white; border-radius: 4px; padding: 8px; font-weight: bold; } "
                           "QPushButton:hover { background-color: #666666; }");
    connect(btnEdit, &QPushButton::clicked, [this, index]() { editEquipment(index); });
    QPushButton* btnDel = new QPushButton("Supprimer");
    btnDel->setStyleSheet("QPushButton { background-color: #d32f2f; color: white; border-radius: 4px; padding: 8px; font-weight: bold; } "
                          "QPushButton:hover { background-color: #f44336; }");
    connect(btnDel, &QPushButton::clicked, [this, index]() { deleteEquipment(index); });
    btnLayout->addWidget(btnEdit);
    btnLayout->addWidget(btnDel);
    layout->addLayout(btnLayout);
    return card;
}

/**
 * @brief InterfacePcClient::editEquipment
 * @details Charge les données d'un équipement existant dans le formulaire en mode édition.
 * @param index Index de l'équipement à modifier dans equipmentsList.
 */
void InterfacePcClient::editEquipment(int index)
{
    if (index < 0 || index >= equipmentsList.size()) return;
    currentEditEquipIndex = index;
    const EquipmentData& eq = equipmentsList[index];
    clearForm();
    couleurActuelle = eq.couleur.isEmpty() ? "#000000" : eq.couleur;
    ui->pushButtonCouleur->setStyleSheet(
                QString("background-color: %1; color: white; border-radius: 4px; padding: 8px 16px; font-weight: bold; border: none;").arg(couleurActuelle)
                );
    ui->nameEdit->setText(eq.nom);
    ui->startAddressEdit->setText(eq.dmxStart);
    for (const ChannelData& cd : eq.canaux) addChannelToForm(&cd);
    ui->stackedWidget->setCurrentWidget(ui->formPage);
}

/**
 * @brief InterfacePcClient::deleteEquipment
 * @details Supprime un équipement après confirmation de l'utilisateur et rafraîchit la grille.
 * @param index Index de l'équipement à supprimer dans equipmentsList.
 */
void InterfacePcClient::deleteEquipment(int index)
{
    if (index < 0 || index >= equipmentsList.size()) return;
    if (QMessageBox::question(this, "Confirmation", "Supprimer cet équipement ?") != QMessageBox::Yes) return;
    if (bdd.supprimerEquipment(equipmentsList[index].idEquipement))
        refreshEquipmentsGrid();
    else
        statusBar()->showMessage("Impossible de supprimer l'équipement", 3000);
}

/**
 * @brief InterfacePcClient::clearForm
 * @details Réinitialise tous les champs du formulaire (nom, adresse, couleur, canaux).
 */
void InterfacePcClient::clearForm()
{
    ui->nameEdit->clear();
    ui->startAddressEdit->clear();
    channelCounter = 0;
    couleurActuelle = "#383838";
    ui->pushButtonCouleur->setStyleSheet("background-color: #383838; color: white; border-radius: 4px; padding: 8px 16px; font-weight: bold; border: none; ");
    ui->channelCountLabel->setText("CANAUX: <span style='color:#107c7c;'>0</span>");
    QLayoutItem* item;
    while ((item = ui->channelsFormLayout->takeAt(0)) != nullptr) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }
}

/**
 * @brief InterfacePcClient::addChannelToForm
 * @details Ajoute dynamiquement un canal DMX au formulaire avec sa description et ses fonctions.
 * @param data Données du canal à restaurer, ou nullptr pour créer un canal vide.
 */
void InterfacePcClient::addChannelToForm(const ChannelData *data)
{
    channelCounter++;
    ui->channelCountLabel->setText(
                QString("CANAUX: <span style='color:#107c7c;'>%1</span>").arg(channelCounter));

    QFrame* channelFrame = new QFrame();
    channelFrame->setObjectName("card");
    QVBoxLayout* cLayout = new QVBoxLayout(channelFrame);

    QHBoxLayout* headerLayout = new QHBoxLayout();
    QLabel*      cTitle       = new QLabel(QString("CANAL #%1").arg(channelCounter));
    cTitle->setObjectName("boldLabel");
    QLineEdit*   cDesc        = new QLineEdit();
    cDesc->setPlaceholderText("Description du canal");
    cDesc->setStyleSheet("background-color: #383838; border: 1px solid #555; color: #ffffff; padding: 8px; border-radius: 4px; focus");
    if (data) cDesc->setText(data->description);
    QPushButton* btnDelChannel = new QPushButton("Supprimer");
    btnDelChannel->setStyleSheet(" QPushButton   { background-color: #d32f2f; color: white; border-radius: 4px; padding: 8px 16px; font-weight: bold;},"
                                 " QPushButton:hover   { background-color: #f44336; }");
    headerLayout->addWidget(cTitle);
    headerLayout->addWidget(cDesc, 1);
    headerLayout->addWidget(btnDelChannel);
    cLayout->addLayout(headerLayout);

    QVBoxLayout* funcsLayout = new QVBoxLayout();
    cLayout->addLayout(funcsLayout);

    QPushButton* btnAddFunc = new QPushButton("+ Ajouter fonction");
    btnAddFunc->setStyleSheet(" QPushButton {background-color: transparent; color: #107c7c; border: 1px dashed #107c7c; padding: 12px;},"
                              " QPushButton:hover { background-color: rgba(16, 124, 124, 0.1); }");
    cLayout->addWidget(btnAddFunc);

    connect(btnDelChannel, &QPushButton::clicked, [this, channelFrame]() {
        channelFrame->deleteLater();
        channelCounter--;
        ui->channelCountLabel->setText(
                    QString("CANAUX: <span style='color:#107c7c;'>%1</span>").arg(channelCounter));
    });
    connect(btnAddFunc, &QPushButton::clicked,
            [this, funcsLayout]() { addFunctionToChannel(funcsLayout, nullptr); });

    if (data && !data->fonctions.isEmpty())
        for (const auto& func : data->fonctions) addFunctionToChannel(funcsLayout, &func);
    else
        addFunctionToChannel(funcsLayout, nullptr);

    ui->channelsFormLayout->addWidget(channelFrame);
}

/**
 * @brief InterfacePcClient::addFunctionToChannel
 * @details Ajoute une ligne de fonction (nom, min, max) dans le layout d'un canal.
 * @param functionsLayout Layout vertical cible dans lequel insérer la ligne.
 * @param data            Données de la fonction à restaurer, ou nullptr pour une ligne vide.
 */
void InterfacePcClient::addFunctionToChannel(QVBoxLayout* functionsLayout, const FunctionData* data)
{
    QWidget*     funcWidget = new QWidget();
    QHBoxLayout* layout     = new QHBoxLayout(funcWidget);
    layout->setContentsMargins(0, 5, 0, 5);

    QLineEdit* fName = new QLineEdit(); fName->setPlaceholderText("Fonction");
    QLineEdit* fMin  = new QLineEdit(); fMin->setPlaceholderText("Min"); fMin->setFixedWidth(60);
    QLineEdit* fMax  = new QLineEdit(); fMax->setPlaceholderText("Max"); fMax->setFixedWidth(60);

    fMax->setStyleSheet("background-color: #383838; border: 1px solid #555; color: #ffffff; padding: 8px; border-radius: 4px;");
    fName->setStyleSheet("background-color: #383838; border: 1px solid #555; color: #ffffff; padding: 8px; border-radius: 4px;");
    fMin->setStyleSheet("background-color: #383838; border: 1px solid #555; color: #ffffff; padding: 8px; border-radius: 4px;");

    if (data) { fName->setText(data->nom); fMin->setText(data->min); fMax->setText(data->max); }

    QPushButton* btnDelFunc = new QPushButton("X");
    btnDelFunc->setObjectName("btnRed");
    btnDelFunc->setStyleSheet("QPushButton#btnRed   { background-color: #d32f2f; color: white; border-radius: 4px; padding: 8px 16px; font-weight: bold; }, "
                              "QPushButton#btnRed:hover   { background-color: #f44336; }");
    connect(btnDelFunc, &QPushButton::clicked, funcWidget, &QWidget::deleteLater);

    layout->addWidget(fName);
    layout->addWidget(fMin);
    layout->addWidget(fMax);
    layout->addWidget(btnDelFunc);
    functionsLayout->addWidget(funcWidget);
}

/**
 * @brief InterfacePcClient::creerSliders
 * @details Recrée entièrement la zone de sliders DMX (un slider vertical 0–255 par canal).
 *        Chaque slider met à jour son label valeur et affiche dynamiquement la fonction
 *        DMX correspondante si le canal est mappé à un équipement.
 * @param nombreCanaux Nombre de sliders à générer.
 */
void InterfacePcClient::creerSliders(int nombreCanaux)
{
    if (QLayout* oldLayout = ui->slidersContainer->layout()) {
        QLayoutItem* item;
        while ((item = oldLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete oldLayout;
    }

    dmxSliders.clear();
    QGridLayout* slidersGrid = new QGridLayout(ui->slidersContainer); // ← maintenant installé correctement
    slidersGrid->setSpacing(10);

    for (int i = 0; i < nombreCanaux; ++i) {
        int dmxChannel = i + 1;
        QFrame* sliderFrame = new QFrame();
        sliderFrame->setObjectName("card");
        sliderFrame->setStyleSheet("QFrame#card { background-color: #3c3c3c; border: 1px solid #555; border-radius: 6px; }");
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

        connect(slider, &QSlider::valueChanged, [this, i, dmxChannel, lVal, lTitle](int val) {
            lVal->setText(QString::number(val));
            if (dmxSliders[i].idCanalDB != -1) {
                QString textToDisplay = dmxSliders[i].descriptionBase;
                for (const auto& f : dmxSliders[i].fonctions) {
                    if (val >= f.min && val <= f.max) {
                        textToDisplay = f.nom;
                        break;
                    }
                }
                lTitle->setText(
                            QString("<b>%1</b><br>"
                                    "<span style='color:%2;'>%3</span><br>"  // <- nom en couleur de l'équipement
                                    "<i style='color:#00e5ff;'>%4</i>")
                            .arg(dmxChannel)
                            .arg(dmxSliders[i].couleur)
                            .arg(dmxSliders[i].nomEquipement)
                            .arg(textToDisplay)
                            );
            }
        });

        sLayout->addWidget(lTitle);
        sLayout->addWidget(slider, 1, Qt::AlignHCenter);
        sLayout->addWidget(lVal);
        slidersGrid->addWidget(sliderFrame, i / 16, i % 16);
        dmxSliders.append({-1, lTitle, slider, lVal, "", "", {}});
    }
}

/**
 * @brief InterfacePcClient::on_pushButtonCouleur_clicked
 * @details Ouvre le sélecteur de couleur Qt et applique la couleur choisie au bouton.
 */
void InterfacePcClient::on_pushButtonCouleur_clicked()
{
    QColor couleur = QColorDialog::getColor(
                QColor(ui->pushButtonCouleur->styleSheet().isEmpty() ? "#FFFFFF" : couleurActuelle),
                this,
                "Choisir une couleur"
                );

    if (couleur.isValid()) {
        couleurActuelle = couleur.name(); // stocke "#RRGGBB"

        // Colorier le bouton pour donner un aperçu visuel
        ui->pushButtonCouleur->setStyleSheet(
                    QString("background-color: %1; color: white; border-radius: 4px; padding: 8px 16px; font-weight: bold; border: none;")
                    .arg(couleurActuelle)
                    );
    }
}

/**
 * @brief InterfacePcClient::on_btnAddChannelGlobal_clicked
 * @details Ajoute un canal vide au formulaire via addChannelToForm(nullptr)
 */
void InterfacePcClient::on_btnAddChannelGlobal_clicked()
{
    addChannelToForm(nullptr);
}

/**
 * @brief InterfacePcClient::on_btnCancel_clicked
 * @details Annule la saisie et retourne à la page de liste.
 */
void InterfacePcClient::on_btnCancel_clicked()
{
    on_btnGoToList_clicked();
}

/**
 * @brief InterfacePcClient::on_btnSave_clicked
 * @details Construit un EquipmentData depuis le formulaire et l'enregistre ou le modifie en BDD.
 *        Retourne à la liste en cas de succès.
 */
void InterfacePcClient::on_btnSave_clicked()
{
    EquipmentData eq;
    eq.nom      = ui->nameEdit->text().isEmpty()         ? "Nouvel Équipement" : ui->nameEdit->text();
    eq.univers  = ui->universCombo->currentText();
    eq.dmxStart = ui->startAddressEdit->text().isEmpty() ? "1"                 : ui->startAddressEdit->text();
    eq.couleur  = couleurActuelle;

    for (int i = 0; i < ui->channelsFormLayout->count(); ++i) {
        QFrame* cFrame = qobject_cast<QFrame*>(ui->channelsFormLayout->itemAt(i)->widget());
        if (!cFrame) continue;
        ChannelData cd;
        QList<QLineEdit*> lineEdits = cFrame->findChildren<QLineEdit*>();
        if (!lineEdits.isEmpty()) {
            cd.description = lineEdits[0]->text();
            for (int j = 1; j + 2 < lineEdits.size(); j += 3)
                cd.fonctions.append({lineEdits[j]->text(), lineEdits[j+1]->text(), lineEdits[j+2]->text()});
        }
        eq.canaux.append(cd);
    }

    int  idU      = ui->universCombo->currentData().toInt();
    bool resultat = (currentEditEquipIndex >= 0)
            ? bdd.modifierEquipment(equipmentsList[currentEditEquipIndex].idEquipement, eq, idU)
            : bdd.enregistrerEquipment(eq, idU);

    if (resultat) {
        refreshEquipmentsGrid();
        on_btnCancel_clicked();
    } else {
        statusBar()->showMessage("Impossible d'ajouter l'équipement", 3000);
    }
}

/**
 * @brief InterfacePcClient::on_scenesUniversCombo_currentIndexChanged
 * @details Recharge les sliders DMX et les associe aux canaux de l'univers sélectionné.
 * @param index Index de l'univers sélectionné dans scenesUniversCombo.
 */
void InterfacePcClient::on_scenesUniversCombo_currentIndexChanged(int index)
{
    int idUnivers = ui->scenesUniversCombo->itemData(index).toInt();
    int nombreCanaux = bdd.recupererCompteurCanaux(idUnivers);
    QMap<int, DmxChannelInfo> mapCanaux = bdd.chargerMapUnivers(idUnivers);

    creerSliders(nombreCanaux);



    for (int i = 0; i < dmxSliders.size(); ++i) {
        int dmxChannel = i + 1;
        dmxSliders[i].idCanalDB       = -1;
        dmxSliders[i].nomEquipement   = "";
        dmxSliders[i].descriptionBase = "";
        dmxSliders[i].fonctions.clear();

        if (mapCanaux.contains(dmxChannel)) {
            const DmxChannelInfo& info    = mapCanaux[dmxChannel];
            dmxSliders[i].idCanalDB       = info.idCanal;
            dmxSliders[i].nomEquipement   = info.nomEquipement;
            dmxSliders[i].couleur         = info.couleur;
            dmxSliders[i].descriptionBase = info.description;
            dmxSliders[i].fonctions       = info.fonctions;
            dmxSliders[i].labelTitre->setStyleSheet("font-size: 10px;");
            emit dmxSliders[i].slider->valueChanged(dmxSliders[i].slider->value());
        } else {
            dmxSliders[i].labelTitre->setText(QString::number(dmxChannel));
            dmxSliders[i].labelTitre->setStyleSheet("font-size: 10px; color: #555;");
            dmxSliders[i].slider->setValue(0);
        }
    }
}

/**
 * @brief InterfacePcClient::on_scenesCombo_currentIndexChanged
 * @details Charge les valeurs DMX de la scène sélectionnée sur les sliders,
 *        ou remet tout à zéro si "Nouvelle Scène" est sélectionné (id = -1).
 * @param index Index de la scène sélectionnée dans scenesCombo.
 */
void InterfacePcClient::on_scenesCombo_currentIndexChanged(int index)
{
    int idScene = ui->scenesCombo->itemData(index).toInt();
    if (idScene == -1) {
        on_btnResetSliders_clicked();
        ui->btnRenameScene->setEnabled(false);
        ui->btnDeleteScene->setEnabled(false);
    } else {
        ui->btnRenameScene->setEnabled(true);
        ui->btnDeleteScene->setEnabled(true);
        QMap<int, int> valeursEnregistrees = bdd.chargerValeursScene(idScene);
        for (int i = 0; i < dmxSliders.size(); ++i) {
            int idCanalDB = dmxSliders[i].idCanalDB;
            dmxSliders[i].slider->setValue(
                        (idCanalDB != -1 && valeursEnregistrees.contains(idCanalDB))
                        ? valeursEnregistrees[idCanalDB] : 0);
        }
    }
}

/**
 * @brief InterfacePcClient::on_btnResetSliders_clicked
 * @details Remet la valeur de tous les sliders DMX à 0.
 */
void InterfacePcClient::on_btnResetSliders_clicked()
{
    for (int i = 0; i < dmxSliders.size(); ++i)
        dmxSliders[i].slider->setValue(0);
}

/**
 * @brief InterfacePcClient::on_btnRenameScene_clicked
 * @details Demande un nouveau nom via QInputDialog et renomme la scène sélectionnée en BDD.
 */
void InterfacePcClient::on_btnRenameScene_clicked()
{
    int idScene = ui->scenesCombo->currentData().toInt();
    if (idScene == -1) {
        statusBar()->showMessage("Aucune scène sélectionnée. Veuillez sélectionner une scène déjà existante !");
    } else {
        bool ok;
        QString nouveauNom = QInputDialog::getText(this, "Renommer la scène",
                                                   "Nouveau nom :", QLineEdit::Normal, ui->scenesCombo->currentText(), &ok);
        if (ok && !nouveauNom.isEmpty() && bdd.renommerScene(idScene, nouveauNom)) {
            refreshScenesList();
            int index = ui->scenesCombo->findData(idScene);
            if (index != -1) ui->scenesCombo->setCurrentIndex(index);
        } else {
            statusBar()->showMessage("Impossible de modifier le nom de la scène");
        }
    }
}

/**
 * @brief InterfacePcClient::on_btnDeleteScene_clicked
 * @details Supprime la scène sélectionnée après confirmation, puis rafraîchit la liste et remet les sliders à zéro.
 */
void InterfacePcClient::on_btnDeleteScene_clicked()
{
    int idScene = ui->scenesCombo->currentData().toInt();
    if (idScene == -1) {
        statusBar()->showMessage("Aucune scène sélectionnée. Veuillez sélectionner une scène déjà existante !");
    } else {
        if (QMessageBox::question(this, "Confirmation", "Supprimer cette scène ?") != QMessageBox::Yes) return;
        if (bdd.supprimerScene(idScene)) {
            refreshScenesList();
            on_btnResetSliders_clicked();
        } else {
            statusBar()->showMessage("Impossible de supprimer la scène");
        }
    }
}

/**
 * @brief InterfacePcClient::on_btnSaveScene_clicked
 * @details Demande un nom, collecte les valeurs des sliders non nuls et enregistre la scène en BDD.
 */
void InterfacePcClient::on_btnSaveScene_clicked()
{
    if (ui->scenesUniversCombo->count() == 0) {
        statusBar()->showMessage("Aucun univers enregistré");
        return;
    } else {
        bool ok;
        QString sceneName = QInputDialog::getText(this, "Nouvelle Scène",
                                                  "Entrez le nom de la scène :", QLineEdit::Normal, "", &ok);
        if (!ok || sceneName.trimmed().isEmpty()) return;

        QMap<int, int> valeursAEnregistrer;
        for (int i = 0; i < dmxSliders.size(); ++i) {
            int idCanalDB = dmxSliders[i].idCanalDB;
            int valeur    = dmxSliders[i].slider->value();
            if (idCanalDB != -1 && valeur != 0)
                valeursAEnregistrer.insert(idCanalDB, valeur);
        }

        if (valeursAEnregistrer.isEmpty()) {
            statusBar()->showMessage("Aucune valeur > 0 à enregistrer");
        } else {

            if (bdd.enregistrerScene(sceneName, valeursAEnregistrer)) {
                statusBar()->showMessage("Scène '" + sceneName + "' sauvegardée !");
                refreshScenesList();
            } else {
                statusBar()->showMessage("Impossible d'enregistrer la scène");
            }
        }
    }
}

/**
 * @brief InterfacePcClient::refreshScenesList
 * @details Recharge et repeuple scenesCombo depuis la BDD
 *        (option "-- Nouvelle Scène --" en tête, id = -1).
 */
void InterfacePcClient::refreshScenesList()
{
    ui->scenesCombo->blockSignals(true);
    ui->scenesCombo->clear();
    ui->scenesCombo->addItem("-- Nouvelle Scène --", -1);
    scenesList = bdd.chargerLesScenes();
    for (const auto& s : scenesList)
        ui->scenesCombo->addItem(s.nomScene, s.idScene);
    ui->scenesCombo->blockSignals(false);
}

/**
 * @brief InterfacePcClient::on_btnConnectTCP_clicked
 * @details Tente une connexion TCP avec l'IP et le port saisis, ou déconnecte si déjà connecté.
 */
void InterfacePcClient::on_btnConnectTCP_clicked()
{
    if (socketClient.state() == QAbstractSocket::UnconnectedState) {
        QString ip   = ui->lineEditIP->text();
        int     port = ui->spinBoxPort->value();
        if (ip.isEmpty() || port <= 0) {
            statusBar()->showMessage("Mauvaise adresse IP ou port invalide !");
        } else {
            socketClient.connectToHost(ip, port);
        }
    } else {
        socketClient.disconnectFromHost();
    }
}

/**
 * @brief InterfacePcClient::onQTcpSocket_connected
 * @details Slot TCP : connexion établie. Passe le bouton en vert et affiche un message de statut.
 */
void InterfacePcClient::onQTcpSocket_connected()
{
    ui->btnConnectTCP->setText("Déconnexion");
    ui->btnConnectTCP->setObjectName("btnGreen");
    ui->btnConnectTCP->setStyleSheet(" QPushButton#btnGreen { background-color: #388e3c;  color: white; border-radius: 4px; padding: 8px 16px; font-weight: bold; border: none; }"
                                     " QPushButton#btnGreen:hover { background-color: #4caf50; }");
    ui->btnConnectTCP->style()->polish(ui->btnConnectTCP);
    statusBar()->showMessage("Connexion au serveur établie", 3000);
}

/**
 * @brief InterfacePcClient::onQTcpSocket_disconnected
 * @details Slot TCP : connexion perdue. Remet le bouton en gris et affiche un message de statut.
 */
void InterfacePcClient::onQTcpSocket_disconnected()
{
    ui->btnConnectTCP->setText("Connexion");
    ui->btnConnectTCP->setObjectName("btnGrey");
    ui->btnConnectTCP->setStyleSheet(" QPushButton#btnGrey  { background-color: #555555;  color: white; border-radius: 4px; padding: 8px 16px; font-weight: bold; border: none; }"
                                     " QPushButton#btnGrey:hover  { background-color: #666666; }");
    ui->btnConnectTCP->style()->polish(ui->btnConnectTCP);
    statusBar()->showMessage("Perte de la connexion au serveur", 3000);
}

/**
 * @brief InterfacePcClient::on_liveScenesList_itemSelectionChanged
 * @details Met à jour le bouton de lancement avec le nom et l'univers de la scène sélectionnée,
 *        ou le désactive si aucune sélection.
 */
void InterfacePcClient::on_liveScenesList_itemSelectionChanged()
{
    if (ui->liveScenesList->selectedItems().isEmpty()) {
        ui->btnLaunchLiveScene->setEnabled(false);
        ui->btnLaunchLiveScene->setText("SÉLECTIONNEZ UNE SCÈNE");
        selectedLiveSceneId = -1;
    } else {
        QListWidgetItem* item = ui->liveScenesList->selectedItems().first();
        selectedLiveSceneId   = item->data(Qt::UserRole).toInt();
        int numeroUnivers     = item->data(Qt::UserRole + 1).toInt();
        ui->btnLaunchLiveScene->setEnabled(true);
        ui->btnLaunchLiveScene->setText(
                    QString("▶ LANCER : %1  —  UNIVERS %2")
                    .arg(item->text().section("     —     ", 0, 0).toUpper())
                    .arg(numeroUnivers)
                    );
    }
}

/**
 * @brief InterfacePcClient::on_btnLaunchLiveScene_clicked
 * @details Lance la scène sélectionnée si une scène valide est sélectionnée (id != -1).
 */
void InterfacePcClient::on_btnLaunchLiveScene_clicked()
{
    if (selectedLiveSceneId != -1) lancerScene(selectedLiveSceneId);
}

/**
 * @brief InterfacePcClient::lancerScene
 * @details Envoie au serveur via TCP un objet JSON {"commande":"P", "idScene": idScene}.
 * @param idScene Identifiant de la scène à lancer.
 */
void InterfacePcClient::lancerScene(int idScene)
{
    QJsonObject obj;
    obj["commande"] = "P";
    obj["idScene"]  = idScene;

    QJsonDocument doc(obj);
    QByteArray data = doc.toJson(QJsonDocument::Compact);

    qDebug() << "Envoi JSON :" << data;

    socketClient.write(data);
}

/**
 * @brief InterfacePcClient::on_filterUniversCombo_currentIndexChanged
 * @details Rafraîchit la grille des équipements selon l'univers sélectionné dans le filtre.
 * @param index Index sélectionné dans filterUniversCombo.
 */
void InterfacePcClient::on_filterUniversCombo_currentIndexChanged(int index)
{
    refreshEquipmentsGrid();
}

/**
 * @brief InterfacePcClient::on_liveUniversCombo_currentIndexChanged
 * @details Applique le filtre d'univers sélectionné sur la liste des scènes live.
 * @param index Index sélectionné dans liveUniversCombo.
 */
void InterfacePcClient::on_liveUniversCombo_currentIndexChanged(int index)
{
    int idFiltre = ui->liveUniversCombo->itemData(index).toInt();
    refreshLiveScenesList(idFiltre);
}

/**
 * @brief InterfacePcClient::refreshLiveScenesList
 * @details Recharge la liste des scènes live en appliquant un filtre optionnel par univers.
 *        Chaque item stocke l'idScene et le numéro d'univers en données utilisateur.
 * @param idUniversFiltre Identifiant de l'univers à filtrer, ou -1 pour tout afficher.
 */
void InterfacePcClient::refreshLiveScenesList(int idUniversFiltre)
{
    ui->liveScenesList->clear();
    ui->btnLaunchLiveScene->setEnabled(false);
    ui->btnLaunchLiveScene->setText("SÉLECTIONNEZ UNE SCÈNE");
    selectedLiveSceneId = -1;

    QList<SceneData> scenes = bdd.chargerLesScenes();

    for (const auto& scene : scenes) {
        // Récupérer le numéro d'univers associé à cette scène
        int numeroUnivers = bdd.getUniversDeScene(scene.idScene);

        // Appliquer le filtre si nécessaire
        if (idUniversFiltre != -1) {
            // Trouver l'idUnivers correspondant au numéro pour comparer
            int idUniversScene = -1;
            for (const auto& u : universList) {
                if (u.numero == numeroUnivers) {
                    idUniversScene = u.idUnivers;
                    break;
                }
            }
            if (idUniversScene != idUniversFiltre)
                continue;
        }

        // Construire le label : "Nom de la scène  —  Univers X
        QString label = QString("%1     —     Univers %2").arg(scene.nomScene).arg(numeroUnivers);

        QListWidgetItem* item = new QListWidgetItem(label);
        item->setData(Qt::UserRole,     scene.idScene);
        item->setData(Qt::UserRole + 1, numeroUnivers);
        ui->liveScenesList->addItem(item);
    }
}

