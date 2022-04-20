#include "dialogprompt.h"
#include "ui_dialogprompt.h"

DialogPrompt::DialogPrompt(QWidget *parent, QString path, Prompt prompt, QString rename) :
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
        this->rename = "";
    } else if (this->prompt == Prompt::F6) {
        this->rename = rename;
        if (this->rename == "..") {
            // TODO forbiden avant, pas de copie ..
        } else {
            ui->lineEdit->setText(this->rename);
        }
        QString msg = "Veuillez modifier (ou pas) et valider le déplacement vers dans \"" + this->path.toLocal8Bit() + "\".";
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
    } else if (this->prompt == Prompt::F6) {

    }
    this->parent->show();
    this->close();
}

QString DialogPrompt::getPath() {
    return this->path;
}
