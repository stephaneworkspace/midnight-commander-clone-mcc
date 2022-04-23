#include "dialogprompt.h"
#include "ui_dialogprompt.h"
#include "mess.h"

DialogPrompt::DialogPrompt(QWidget *parent, QString path, Prompt prompt, QString rename, QString origin) :
    QDialog(parent),
    ui(new Ui::DialogPrompt)
{
    this->parent = parent;
    this->path = path;
    this->prompt = prompt;
    this->origin = origin + rename;
    ui->setupUi(this);
    if (this->prompt == Prompt::F7) {
        QString msg = "Veuillez saisir le répertoire à crée dans \"" + this->path + "\".";
        ui->label->setText(msg);
        this->rename = "";
    } else if (this->prompt == Prompt::F6) {
        this->rename = rename;
        if (this->rename == "" || this->rename == "..") {
            // TODO tester
            this->reject();
        } else {
            ui->lineEdit->setText(this->rename);
        }
        QString msg = "Veuillez valider le déplacement de \"" + this->origin +"\" avec la saisie vers \"" + this->path + "\"."; // TODO custom dir/file...
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
        // F7 Makedir
        this->path = this->path + ui->lineEdit->text();
        mkdir(this->path.toLocal8Bit().data(), 0777); // TODO analyse int result
    } else if (this->prompt == Prompt::F6) {
        // F6 Move/Rename
        Mess::DispMessQString("ui->lineEdit->text()",ui->lineEdit->text());
        Mess::DispMessQString("this->path",this->path);
        Mess::DispMessQString("this->origin",this->origin);
    }
    this->parent->show();
    this->close();
}

QString DialogPrompt::getPath() {
    return this->path;
}
