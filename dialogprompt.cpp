#include "dialogprompt.h"
#include "ui_dialogprompt.h"

DialogPrompt::DialogPrompt(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogPrompt)
{
    this->parent = parent;
    ui->setupUi(this);
}

DialogPrompt::~DialogPrompt()
{
    delete ui;
}

void DialogPrompt::on_buttonBox_rejected()
{
    this->parent->show();
    this->close();
}


void DialogPrompt::on_buttonBox_accepted()
{

}

