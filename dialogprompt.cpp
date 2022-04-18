#include "dialogprompt.h"
#include "ui_dialogprompt.h"

DialogPrompt::DialogPrompt(QWidget *parent, QString path, Prompt prompt) :
    QDialog(parent),
    ui(new Ui::DialogPrompt)
{
    this->parent = parent;
    this->path = path;
    this->prompt = prompt;
    ui->setupUi(this);
    if (this->prompt == Prompt::F7) {
        QString msg = "Veuillez saisir le répertoire à crée dans \"" + this->path.toLocal8Bit() + "\".";
        ui->label->setText(msg);
    }
}

DialogPrompt::~DialogPrompt()
{
    delete ui;
}

void DialogPrompt::on_buttonBox_rejected()
{
    this->path = "";
    this->parent->show();
    this->close();
}


void DialogPrompt::on_buttonBox_accepted()
{
    if (this->prompt == Prompt::F7) {
        this->path = this->path + ui->lineEdit->text();
        mkdir(this->path.toLocal8Bit(), 0777); // TODO analyse int result
    }
    this->parent->show();
    this->close();
}

QString DialogPrompt::getPath() {
    return this->path;
}
