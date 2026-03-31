#ifndef INTERFACEPCCLIENT_H
#define INTERFACEPCCLIENT_H

#include "accessbdd.h"
#include <QMainWindow>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QLineEdit>
#include <QScrollArea>
#include <QFrame>
#include <QComboBox>
#include <QListWidget>
#include <QSlider>
#include <QTcpSocket>
#include <QBuffer>
#include <QInputDialog>
#include <QStatusBar>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

struct SliderWidgetSet {
    int      idCanalDB = -1;
    QLabel*  labelTitre;
    QSlider* slider;
    QLabel*  labelValeur;
    QString  nomEquipement;
    QString  descriptionBase;
    QList<DmxFunctionInfo> fonctions;
};

class InterfacePcClient : public QMainWindow {
    Q_OBJECT

public:
    explicit InterfacePcClient(QWidget *parent = nullptr);
    ~InterfacePcClient();

private slots:
    void showAddForm();
    void showList();
    void addUnivers();
    void editUnivers();
    void deleteUnivers();
    void onUniversSelectionChanged();
    void saveEquipment();
    void clearForm();
    void showScenesPage();
    void onScenesUniversChanged();
    void saveCurrentScene();
    void refreshScenesList();
    void onSceneSelectionChanged();
    void resetSliders();
    void onRenameSceneClicked();
    void onDeleteSceneClicked();
    void showLivePage();
    void onLiveSceneSelected();
    void onLaunchButtonClicked();
    void lancerScene(int idScene);
    void onQTcpSocket_connected();
    void onQTcpSocket_disconnected();
    void on_btnConnect_clicked();

private:
    void setupUi();
    void setupStyle();
    void addChannelToForm(const ChannelData* data = nullptr);
    void addFunctionToChannel(QVBoxLayout* functionsLayout, const FunctionData* data = nullptr);
    void refreshUniversList();
    void refreshEquipmentsGrid();
    void editEquipment(int index);
    void deleteEquipment(int index);
    QFrame* createEquipmentCard(const EquipmentData& eq, int index);

    AccessBDD              bdd;
    QList<UniversData>     universList;
    QList<EquipmentData>   equipmentsList;
    QList<SceneData>       scenesList;
    QList<SliderWidgetSet> dmxSliders;
    int currentEditEquipIndex = -1;
    int channelCounter        = 0;
    int selectedLiveSceneId   = -1;

    QStackedWidget* stackedWidget;
    QWidget*        listPage;
    QWidget*        formPage;
    QWidget*        scenesPage;
    QWidget*        livePage;

    QLabel*      statusLabel;

    QListWidget* uiUniversList;
    QPushButton* btnEditUnivers;
    QPushButton* btnDeleteUnivers;

    QGridLayout* equipmentsGrid;
    QWidget*     equipmentsContainer;

    QLineEdit*   nameEdit;
    QComboBox*   universCombo;
    QLineEdit*   startAddressEdit;
    QLabel*      channelCountLabel;
    QVBoxLayout* channelsFormLayout;

    QComboBox*   scenesUniversCombo;
    QComboBox*   scenesCombo;
    QPushButton* btnRenameScene;
    QPushButton* btnDeleteScene;
    QPushButton* btnResetSliders;

    QListWidget* liveScenesList;
    QPushButton* btnLaunchLiveScene;
    QLineEdit*   lineEditIP;
    QSpinBox*    spinBoxPort;
    QPushButton* btnConnectTCP;

    QTcpSocket socketClient;
};

#endif // INTERFACEPCCLIENT_H
