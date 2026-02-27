#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QList>
#include <QLabel>
#include <QFrame>
#include <QHBoxLayout>

struct DmxFunctionData { QString name; int min; int max; };
struct DmxChannelData { int number; QString description; QList<DmxFunctionData> functions; };
struct Equipment { QString name; QList<DmxChannelData> channels; };

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent=nullptr);

private slots:
    void showAddView() { stack->setCurrentIndex(1); }
    void showListView() { stack->setCurrentIndex(0); }
    void addChannelWidget();
    void saveEquipment();

private:
    QStackedWidget *stack;
    QVBoxLayout *listLayout, *formChannelsLayout;
    QLineEdit *eqNameInput;
    QScrollArea *scrollChannels;
    QList<Equipment> database;

    QWidget* createListView();
    QWidget* createAddView();
    void refreshUiList();
    DmxChannelData getChannelDataFromWidget(QWidget *card);

    // Helpers
    QLabel* L(const QString &txt,int sz=13,bool bold=false);
    QPushButton* B(const QString &txt,const QString &obj="");
    QLineEdit* E(const QString &ph);
    QWidget* createFunctionWidget();
    void setupStyles();
};

#endif
