#include "keips.h"
#include "ui_keips.h"

KEIPS::KEIPS(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::KEIPS)
{
    ui->setupUi(this);
}

KEIPS::~KEIPS()
{
    delete ui;
}
