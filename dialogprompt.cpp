#include "dialogprompt.h"
#include "ui_dialogprompt.h"

DialogPrompt::DialogPrompt(QWidget *parent, QString path) :
    QDialog(parent),
    ui(new Ui::DialogPrompt)
{
    this->parent = parent;
    this->path = path;
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
    this->path = this->path + ui->lineEdit->text(); // TODO / final ?
    mkdir(this->path.toLocal8Bit(), 0777); // TODO analyse int result
    this->parent->show();
    this->close();
}

QString DialogPrompt::getPath() {
    return this->path;
}
