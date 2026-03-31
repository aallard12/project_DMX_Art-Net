#include "dialogunivers.h"
#include "ui_dialogunivers.h"

DialogUnivers::DialogUnivers(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogUnivers)
{
    ui->setupUi(this);
    QRegularExpression rx("^192\\.168\\.1\\.([1-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-4])$");
    ui->ipEdit->setValidator(new QRegularExpressionValidator(rx, this));
    ui->ipEdit->setPlaceholderText("192.168.1.X");
}

DialogUnivers::~DialogUnivers()
{
    delete ui;
}

int DialogUnivers::getNumUnivers()
{
    return ui->numEdit->text().toInt();
}

QString DialogUnivers::getIpUnivers()
{
    return ui->ipEdit->text();
}
