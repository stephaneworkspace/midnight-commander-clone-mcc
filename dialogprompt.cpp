#include "dialogprompt.h"
#include "ui_dialogprompt.h"

DialogPrompt::DialogPrompt(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogPrompt)
{
    ui->setupUi(this);
}

DialogPrompt::~DialogPrompt()
{
    delete ui;
}
