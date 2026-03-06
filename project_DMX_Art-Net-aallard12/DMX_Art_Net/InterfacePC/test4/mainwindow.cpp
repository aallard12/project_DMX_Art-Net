#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupStyles();
    stack = new QStackedWidget();
    stack->addWidget(createListView());
    stack->addWidget(createAddView());
    setCentralWidget(stack);
    resize(1000,800);
}

// ---- STYLES ----
void MainWindow::setupStyles(){
    setStyleSheet(R"(
        QMainWindow{background:#0a0e1a;}
        QLabel{color:#e2e8f0;font-family:'Outfit';}
        QLineEdit{background:#131829;border:2px solid #1e293b;color:white;padding:10px;border-radius:8px;font-size:13px;}
        QLineEdit:focus{border:2px solid #6366f1;background:#1a1f35;}
        QPushButton{background:#6366f1;color:white;font-weight:bold;border-radius:8px;padding:12px;font-size:14px;border:none;}
        QPushButton:hover{background:#818cf8;}
        QPushButton#secondary{background:#1e293b;border:1px solid #334155;color:#94a3b8;}
        QPushButton#secondary:hover{background:#334155;color:white;}
        QPushButton#btnSave{background:#10b981;}
        QPushButton#btnSave:hover{background:#34d399;}
        QFrame#card{background:#131829;border:1px solid #1e293b;border-radius:12px;}
        QScrollArea{border:none;background:transparent;}
    )");
}

// ---- HELPERS ----
QLabel* MainWindow::L(const QString &t,int sz,bool b){ QLabel *l=new QLabel(t); l->setStyleSheet(QString("font-size:%1px;%2color:white;").arg(sz).arg(b?"font-weight:bold;":"")); return l; }
QPushButton* MainWindow::B(const QString &t,const QString &o){ QPushButton* b=new QPushButton(t); if(!o.isEmpty()) b->setObjectName(o); return b; }
QLineEdit* MainWindow::E(const QString &p){ QLineEdit* e=new QLineEdit(); e->setPlaceholderText(p); return e; }

// Fonction DMX
QWidget* MainWindow::createFunctionWidget(){
    QWidget *w=new QWidget(); QHBoxLayout *l=new QHBoxLayout(w);
    l->addWidget(E("Nom")); l->addWidget(E("Min")); l->addWidget(E("Max"));
    return w;
}

// ---- LIST & ADD VIEWS ----
QWidget* MainWindow::createListView(){
    QWidget *w=new QWidget(); QVBoxLayout *lay=new QVBoxLayout(w); lay->setContentsMargins(40,40,40,40);
    QHBoxLayout *h=new QHBoxLayout();
    h->addWidget(L("Mes Équipements",28,true));
    QPushButton *add=B("+ Nouvel Équipement"); add->setFixedWidth(200);
    connect(add,&QPushButton::clicked,this,&MainWindow::showAddView);
    h->addStretch(); h->addWidget(add); lay->addLayout(h);
    QScrollArea *s=new QScrollArea(); s->setWidgetResizable(true);
    QWidget *c=new QWidget(); listLayout=new QVBoxLayout(c); listLayout->setAlignment(Qt::AlignTop); s->setWidget(c);
    lay->addSpacing(20); lay->addWidget(s);
    return w;
}

QWidget* MainWindow::createAddView(){
    QWidget *w=new QWidget(); QVBoxLayout *lay=new QVBoxLayout(w); lay->setContentsMargins(40,40,40,40);
    lay->addWidget(L("Configuration de l'appareil",22,true));
    eqNameInput=E("Nom de l'équipement"); lay->addWidget(eqNameInput);
    lay->addSpacing(20); lay->addWidget(L("CANAUX DMX",14,true));

    scrollChannels=new QScrollArea(); scrollChannels->setWidgetResizable(true);
    QWidget *c=new QWidget(); formChannelsLayout=new QVBoxLayout(c); formChannelsLayout->setAlignment(Qt::AlignTop); scrollChannels->setWidget(c);
    lay->addWidget(scrollChannels);

    QPushButton *addChan=B("+ Ajouter un Canal","secondary");
    connect(addChan,&QPushButton::clicked,this,&MainWindow::addChannelWidget);
    lay->addWidget(addChan);

    QHBoxLayout *foot=new QHBoxLayout();
    QPushButton *back=B("Annuler","secondary"); connect(back,&QPushButton::clicked,this,&MainWindow::showListView);
    QPushButton *save=B("Enregistrer","btnSave"); connect(save,&QPushButton::clicked,this,&MainWindow::saveEquipment);
    foot->addWidget(back); foot->addWidget(save); lay->addSpacing(20); lay->addLayout(foot);

    return w;
}

// ---- AJOUT DE CANAL ----
void MainWindow::addChannelWidget(){
    QFrame *c=new QFrame(); c->setObjectName("card"); c->setMinimumHeight(100); QVBoxLayout *l=new QVBoxLayout(c);
    QHBoxLayout *h=new QHBoxLayout();
    h->addWidget(E("N°")); h->addWidget(E("Description")); l->addLayout(h);

    QVBoxLayout *fL=new QVBoxLayout(); l->addLayout(fL);
    QPushButton *addF=B("+ Ajouter Fonction","secondary");
    connect(addF,&QPushButton::clicked,[=](){ fL->addWidget(createFunctionWidget()); });
    l->addWidget(addF);

    formChannelsLayout->addWidget(c);
}

// ---- DATA ↔ UI ----
DmxChannelData MainWindow::getChannelDataFromWidget(QWidget *card){
    DmxChannelData ch; auto edits=card->findChildren<QLineEdit*>(); ch.number=edits[0]->text().toInt(); ch.description=edits[1]->text();
    QVBoxLayout *fL=nullptr;
    for(int i=0;i<card->layout()->count();++i){ if(auto v=qobject_cast<QVBoxLayout*>(card->layout()->itemAt(i)->layout())){ fL=v; break; } }
    if(fL) for(int i=0;i<fL->count();++i){ QWidget*w=fL->itemAt(i)->widget(); if(!w) continue; auto ed=w->findChildren<QLineEdit*>(); if(ed[0]->text().isEmpty()) continue;
        DmxFunctionData f{ed[0]->text(),ed[1]->text().toInt(),ed[2]->text().toInt()}; ch.functions.append(f);
    }
    return ch;
}

// ---- SAUVEGARDE ----
void MainWindow::saveEquipment(){
    if(eqNameInput->text().isEmpty()) return;
    Equipment eq; eq.name=eqNameInput->text();
    for(int i=0;i<formChannelsLayout->count();++i){ QWidget *c=formChannelsLayout->itemAt(i)->widget(); if(c) eq.channels.append(getChannelDataFromWidget(c)); }
    database.append(eq);

    // Reset
    QLayoutItem *child; eqNameInput->clear();
    while((child=formChannelsLayout->takeAt(0))){ delete child->widget(); delete child; }
    refreshUiList(); showListView();
}

// ---- MISE À JOUR LISTE ----
void MainWindow::refreshUiList(){
    QLayoutItem *child; while((child=listLayout->takeAt(0))){ delete child->widget(); delete child; }
    for(const auto &eq:database){
        QFrame *f=new QFrame(); f->setObjectName("card"); f->setFixedHeight(80); QHBoxLayout *h=new QHBoxLayout(f);
        h->addWidget(L(eq.name,16,true));
        QLabel *b=L(QString("%1 CANAUX").arg(eq.channels.count()),10,true);
        b->setStyleSheet("background-color:#312e81;color:#818cf8;padding:5px;border-radius:4px;");
        h->addStretch(); h->addWidget(b);
        listLayout->addWidget(f);
    }
}
