#include "interfacepcclient.h"
#include "ui_interfacepcclient.h"

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

InterfacePcClient::~InterfacePcClient()
{
    delete ui;
}


void InterfacePcClient::on_btnGoToList_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->listPage);
    refreshUniversList();
    refreshEquipmentsGrid();
}


void InterfacePcClient::on_btnAddMain_clicked()
{
    currentEditEquipIndex = -1;
    clearForm();
    ui->stackedWidget->setCurrentWidget(ui->formPage);
}


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


void InterfacePcClient::on_btnGoToLive_clicked()
{
    ui->liveScenesList->clear();
    ui->btnLaunchLiveScene->setEnabled(false);
    ui->btnLaunchLiveScene->setText("SÉLECTIONNEZ UNE SCÈNE");
    selectedLiveSceneId = -1;
    for (const auto& scene : bdd.chargerLesScenes()) {
        QListWidgetItem* item = new QListWidgetItem(scene.nomScene);
        item->setData(Qt::UserRole, scene.idScene);
        ui->liveScenesList->addItem(item);
    }
    ui->stackedWidget->setCurrentWidget(ui->livePage);
}


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


void InterfacePcClient::refreshUniversList()
{
    ui->uiUniversList->clear();
    ui->universCombo->clear();
    universList = bdd.chargerUnivers();
    for (const auto& u : universList) {
        QString display = QString("Univers %1 (%2)").arg(u.numero).arg(u.ip);
        ui->uiUniversList->addItem(display);
        ui->universCombo->addItem(display, u.idUnivers);
    }
    ui->btnEditUnivers->setEnabled(false);
    ui->btnDeleteUnivers->setEnabled(false);
}


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

void InterfacePcClient::on_uiUniversList_currentRowChanged(int currentRow)
{
    bool hasSelection = ui->uiUniversList->currentRow() >= 0;
    ui->btnEditUnivers->setEnabled(hasSelection);
    ui->btnDeleteUnivers->setEnabled(hasSelection);
}

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
    for (int i = 0; i < equipmentsList.size(); ++i)
        ui->equipmentsGrid->addWidget(createEquipmentCard(equipmentsList[i], i), i / 4, i % 4);
}

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

void InterfacePcClient::deleteEquipment(int index)
{
    if (index < 0 || index >= equipmentsList.size()) return;
    if (QMessageBox::question(this, "Confirmation", "Supprimer cet équipement ?") != QMessageBox::Yes) return;
    if (bdd.supprimerEquipment(equipmentsList[index].idEquipement))
        refreshEquipmentsGrid();
    else
        statusBar()->showMessage("Impossible de supprimer l'équipement", 3000);
}

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


void InterfacePcClient::on_btnAddChannelGlobal_clicked()
{
    addChannelToForm(nullptr);
}


void InterfacePcClient::on_btnCancel_clicked()
{
    on_btnGoToList_clicked();
}


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

void InterfacePcClient::on_scenesUniversCombo_currentIndexChanged(int index)
{
    int idUnivers = ui->scenesUniversCombo->itemData(index).toInt();
    int nombreCanaux = bdd.recupererCompteurCanaux(idUnivers);
    creerSliders(nombreCanaux);

    QMap<int, DmxChannelInfo> mapCanaux = bdd.chargerMapUnivers(idUnivers);

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


void InterfacePcClient::on_btnResetSliders_clicked()
{
    for (int i = 0; i < dmxSliders.size(); ++i)
        dmxSliders[i].slider->setValue(0);
}


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

void InterfacePcClient::onQTcpSocket_connected()
{
    ui->btnConnectTCP->setText("Déconnexion");
    ui->btnConnectTCP->setObjectName("btnGreen");
    ui->btnConnectTCP->setStyleSheet(" QPushButton#btnGreen { background-color: #388e3c;  color: white; border-radius: 4px; padding: 8px 16px; font-weight: bold; border: none; }"
                                     " QPushButton#btnGreen:hover { background-color: #4caf50; }");
    ui->btnConnectTCP->style()->polish(ui->btnConnectTCP);
    statusBar()->showMessage("Connexion au serveur établie", 3000);
}

void InterfacePcClient::onQTcpSocket_disconnected()
{
    ui->btnConnectTCP->setText("Connexion");
    ui->btnConnectTCP->setObjectName("btnGrey");
    ui->btnConnectTCP->setStyleSheet(" QPushButton#btnGrey  { background-color: #555555;  color: white; border-radius: 4px; padding: 8px 16px; font-weight: bold; border: none; }"
                                     " QPushButton#btnGrey:hover  { background-color: #666666; }");
    ui->btnConnectTCP->style()->polish(ui->btnConnectTCP);
    statusBar()->showMessage("Perte de la connexion au serveur", 3000);
}


void InterfacePcClient::on_liveScenesList_itemSelectionChanged()
{
    if (ui->liveScenesList->selectedItems().isEmpty()) {
        ui->btnLaunchLiveScene->setEnabled(false);
        ui->btnLaunchLiveScene->setText("SÉLECTIONNEZ UNE SCÈNE");
        selectedLiveSceneId = -1;
    } else {
        QListWidgetItem* item = ui->liveScenesList->selectedItems().first();
        selectedLiveSceneId   = item->data(Qt::UserRole).toInt();
        ui->btnLaunchLiveScene->setEnabled(true);
        ui->btnLaunchLiveScene->setText("▶ LANCER : " + item->text().toUpper());
    }
}


void InterfacePcClient::on_btnLaunchLiveScene_clicked()
{
    if (selectedLiveSceneId != -1) lancerScene(selectedLiveSceneId);
}

void InterfacePcClient::lancerScene(int idScene)
{
    // quint16  taille   = 0;
    // QChar     commande('P');
    // QBuffer  tampon;
    // tampon.open(QIODevice::WriteOnly);
    // QDataStream out(&tampon);
    // out << taille << commande << idScene;
    // qDebug() << "Envoi trame :" << QChar(commande) << "Scene ID:" << idScene;
    // taille = static_cast<quint16>(tampon.size()) - sizeof(taille);
    // tampon.seek(0);
    // out << taille;
    // socketClient.write(tampon.buffer());
    QJsonObject obj;
    obj["commande"] = "P";
    obj["idScene"]  = idScene;

    QJsonDocument doc(obj);
    QByteArray data = doc.toJson(QJsonDocument::Compact);

    qDebug() << "Envoi JSON :" << data;

    socketClient.write(data);
}




