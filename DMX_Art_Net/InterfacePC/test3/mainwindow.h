#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QGroupBox>
#include <QComboBox>
#include <QPushButton>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMap>
#include <QLineEdit>

// --- Widget pour un seul canal DMX ---
class ChannelWidget : public QWidget {
    Q_OBJECT
public:
    ChannelWidget(QString function, int addr, int val, QWidget *parent = nullptr);

signals:
    void valueChanged(int addr, int val);

private slots:
    void onSliderMoved(int val);
    void onSpinChanged(int val);

private:
    int address;
    QSlider *slider;
    QSpinBox *spin;
};

// --- Widget pour un équipement (groupe de canaux) ---
class FixtureWidget : public QGroupBox {
    Q_OBJECT
public:
    FixtureWidget(QString name, QWidget *parent = nullptr);
};

// --- Fenêtre Principale ---
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void loadScene(int index);
    void saveCurrentScene();
    void onChannelChanged(int addr, int val);
    void addFixtureDialog();

private:
    void setupUI();
    void initDatabase();
    void clearInterface();

    QSqlDatabase db;
    unsigned char dmxBuffer[513]; // Stockage des 512 canaux
    int currentSceneId = 1;
    int currentUniverse = 1;

    // Éléments UI
    QWidget *centralWidget;
    QHBoxLayout *fixtureLayout;
    QComboBox *sceneCombo;
    QComboBox *universeCombo;
    QLineEdit *sceneNameEdit;
};

#endif // MAINWINDOW_H
