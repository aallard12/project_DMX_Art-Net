#ifndef INTERFACEPCCLIENT_H
#define INTERFACEPCCLIENT_H

#include <QtTest>
#include <QCoreApplication>
#include "accessbdd.h"

#include <QMainWindow>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QScrollArea>
#include <QFrame>
#include <QComboBox>
#include <QListWidget>
#include <QSlider>
#include <QMap>
#include <QInputDialog>
#include <QSizePolicy>
#include <QListWidget>
#include <QAbstractSocket>
#include <QBuffer>
#include <QDataStream>
#include <QSpinBox>
#include <QTcpSocket>

// -----------------------------------

class interfacepcclient_test : public QMainWindow {
    Q_OBJECT

public:
    interfacepcclient_test(QWidget *parent = nullptr);
    ~interfacepcclient_test();

private slots:
    void showAddForm();
    void showList();
    void addUnivers();
    void editUnivers();
    void deleteUnivers();
    void onUniversSelectionChanged();

    void saveEquipment();
    void clearForm();

    // Créer des scènes
    void showScenesPage();
    void onScenesUniversChanged();
    void saveCurrentScene();
    void refreshScenesList();
    void onSceneSelectionChanged();
    void resetSliders();
    void onRenameSceneClicked();
    void onDeleteSceneClicked();

    void showLivePage();
    void onLiveSceneSelected();          // Quand on clique sur une scène dans la liste
    void onLaunchButtonClicked();
    void lancerScene(int idScene);

    void onQTcpSocket_connected();
    void onQTcpSocket_disconnected();
    void on_btnConnect_clicked();

private:
    void setupUi();
    void setupStyle();

    // Nouvelles méthodes pour gérer la dynamique des formulaires et de la grille
    void addChannelToForm(const ChannelData* data = nullptr);
    void addFunctionToChannel(QVBoxLayout* functionsLayout, const FunctionData* data = nullptr);
    void refreshUniversList();
    void refreshEquipmentsGrid();
    void editEquipment(int index);
    void deleteEquipment(int index);
    QFrame* createEquipmentCard(const EquipmentData& eq, int index);

    // Data
    QList<UniversData> universList;
    QList<EquipmentData> equipmentsList; // Stockage réel des équipements
    int currentEditEquipIndex;           // Index de l'équipement en cours de modification (-1 si nouveau)

    // Navigation
    QStackedWidget* stackedWidget;
    QWidget* listPage;
    QWidget* formPage;

    // Créer des scènes
    QWidget* scenesPage;
    QComboBox* scenesUniversCombo;
    struct SliderWidgetSet {
        int idCanalDB = -1; // Pour l'enregistrement futur
        QLabel* labelTitre;
        QSlider* slider;
        QLabel* labelValeur;
        QString nomEquipement;
        QString descriptionBase;
        QList<DmxFunctionInfo> fonctions;
    };
    QList<SliderWidgetSet> dmxSliders;

    QComboBox* scenesCombo;
    QPushButton* btnResetSliders;
    QPushButton* btnRenameScene;
    QPushButton* btnDeleteScene;

    QList<SceneData> scenesList;

    // List Page Elements
    QListWidget* uiUniversList;
    QPushButton* btnEditUnivers;
    QPushButton* btnDeleteUnivers;
    QGridLayout* equipmentsGrid;
    QWidget* equipmentsContainer;

    QWidget* livePage;
    QGridLayout* liveScenesLayout;
    QListWidget* liveScenesList;         // La liste des scènes
    QPushButton* btnLaunchLiveScene;     // Le gros bouton "GO"
    int selectedLiveSceneId = -1;

    // Eléments d'interface TCP pour le mode Live
    QLineEdit* lineEditIP;
    QSpinBox* spinBoxPort;
    QPushButton* btnConnectTCP;

    // La socket pour communiquer avec le camarade
    QTcpSocket socketClient;

    // Form Page Elements
    QLineEdit* nameEdit;
    QComboBox* universCombo;
    QLineEdit* startAddressEdit;
    QLabel* channelCountLabel;
    QVBoxLayout* channelsFormLayout;

    int channelCounter;
    AccessBDD bdd;

};

#endif // INTERFACEPCCLIENT_H
