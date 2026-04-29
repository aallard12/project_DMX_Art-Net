/**
 * @file interfacepcclient.h
 * @brief Déclaration de la classe InterfacePcClient
 * @author Antoine ALLARD
 * @date 10/04/2026
 */

#ifndef INTERFACEPCCLIENT_H
#define INTERFACEPCCLIENT_H

#include "accessbdd.h"
#include "dialogunivers.h"
#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QSlider>
#include <QTcpSocket>
#include <QBuffer>
#include <QJsonObject>
#include <QJsonDocument>
#include <QColorDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class InterfacePcClient; }
QT_END_NAMESPACE

/**
 * @struct SliderWidgetSet
 * @brief Structure regroupant les widgets et données nécessaires à la gestion d'un canal DMX via un slider
 */
struct SliderWidgetSet {
    int      idCanalDB = -1;
    QLabel*  labelTitre;
    QSlider* slider;
    QLabel*  labelValeur;
    QString  nomEquipement;
    QString  descriptionBase;
    QString couleur;
    QList<DmxFunctionInfo> fonctions;
};

class InterfacePcClient : public QMainWindow
{
    Q_OBJECT

public:
    InterfacePcClient(QWidget *parent = nullptr);
    ~InterfacePcClient();

private slots:
    void on_btnGoToList_clicked();
    void on_btnAddMain_clicked();
    void on_btnGoToScenes_clicked();
    void on_btnGoToLive_clicked();
    void on_filterUniversCombo_currentIndexChanged(int index);

    void on_btnAddUnivers_clicked();
    void refreshUniversList();
    void on_btnEditUnivers_clicked();
    void on_btnDeleteUnivers_clicked();
    void on_uiUniversList_currentRowChanged(int currentRow);
    void clearForm();

    void on_pushButtonCouleur_clicked();
    void on_btnAddChannelGlobal_clicked();
    void on_btnCancel_clicked();
    void on_btnSave_clicked();

    void on_scenesUniversCombo_currentIndexChanged(int index);
    void on_scenesCombo_currentIndexChanged(int index);
    void on_btnResetSliders_clicked();
    void on_btnRenameScene_clicked();
    void on_btnDeleteScene_clicked();
    void on_btnSaveScene_clicked();
    void refreshScenesList();

    void on_btnConnectTCP_clicked();
    void onQTcpSocket_connected();
    void onQTcpSocket_disconnected();
    void on_liveScenesList_itemSelectionChanged();
    void on_btnLaunchLiveScene_clicked();
    void lancerScene(int idScene);
    void on_liveUniversCombo_currentIndexChanged(int index);
    void refreshLiveScenesList(int idUniversFiltre);

private:
    /// Pointeur vers l'interface utilisateur générée par Qt Designer
    Ui::InterfacePcClient *ui;
    /// Composition avec la classe AccessBDD pour accéder à la base de données
    AccessBDD bdd;
    /// Composition avec la classe QTcpSocket pour communiquer via le protocole TCP en étant client
    QTcpSocket socketClient;
    /// Liste stockant les données des univers récupérées depuis la base de données
    QList<UniversData>     universList;
    /// Liste stockant les équipements DMX actuellement chargés en mémoire
    QList<EquipmentData>   equipmentsList;
    /// Liste stockant l'ensemble des scènes enregistrées
    QList<SceneData>       scenesList;
    /// Liste regroupant les ensembles de widgets (sliders et labels) pour le contrôle DMX
    QList<SliderWidgetSet> dmxSliders;
    /// Index de l'équipement en cours de modification dans la liste (-1 si aucun)
    int currentEditEquipIndex = -1;
    /// Compteur utilisé pour l'indexation et la gestion dynamique des canaux dans les formulaires
    int channelCounter        = 0;
    /// Identifiant de la scène actuellement sélectionnée dans le mode Live
    int selectedLiveSceneId   = -1;
    /// Stockage du code couleur hexadécimal sélectionné pour l'équipement
    QString couleurActuelle = "#FFFFFF";
    /// Label affiché dans la barre d'état pour indiquer l'état de la connexion ou les erreurs
    QLabel*      statusLabel;

    void refreshEquipmentsGrid();
    QFrame* createEquipmentCard(const EquipmentData& eq, int index);
    void editEquipment(int index);
    void deleteEquipment(int index);
    void addChannelToForm(const ChannelData* data = nullptr);
    void addFunctionToChannel(QVBoxLayout* functionsLayout, const FunctionData* data = nullptr);
    void creerSliders(int nombreCanaux);
};
#endif // INTERFACEPCCLIENT_H
