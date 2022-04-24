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
            // TODO tester et Mess
            this->reject();
        } else {
            QString lastIndex = "/";
            int lastIndexOfSlash = this->rename.lastIndexOf(lastIndex);
            if (lastIndexOfSlash > 0) {
                this->swDir = true;
                ui->lineEdit->setText(this->rename.split('/')[0]);
            } else if (lastIndexOfSlash == 0){
                // 0 à la position 0 c'est faux
                // TODO Mess déplacement impossible
                this->reject();
            } else {
                // -1 n'existe pas
                this->swDir = false;
                ui->lineEdit->setText(this->rename);
            }
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
        if (ui->lineEdit->text() == "") {
            Mess::DispMessQString("Erreur", "Votre saisie est vide");
            this->reject();
        } else {
            this->path = this->path + ui->lineEdit->text();
            try {
                fs::create_directories(this->path.toLocal8Bit().constData());
            } catch(fs::filesystem_error& e) {
                Mess::DispMess(e);
                this->reject();
            } catch(std::bad_alloc& e) {
                Mess::DispMess(e);
                this->reject();
            } catch (std::exception& e) { // Not using fs::filesystem_error since std::bad_alloc can throw too.
                // Handle exception or use error code overload of fs::copy.
                Mess::DispMess(e);
                this->reject();
            }
        }
    } else if (this->prompt == Prompt::F6) {
        // F6 Move/Rename
        if (ui->lineEdit->text() == "") {
            Mess::DispMessQString("Erreur", "Votre saisie est vide");
            this->reject();
        } else {
            try {
                QString from = this->origin;
                QByteArray from_ba = from.toLocal8Bit();
                const char* from_cstr = from_ba.constData();
                QString to;
                if (swDir) {
                    to = this->path + ui->lineEdit->text() + "/";
                } else {
                    to = this->path + ui->lineEdit->text();
                }
                QByteArray to_ba = to.toLocal8Bit();
                const char *to_cstr = to_ba.constData();
                fs::rename(from_cstr, to_cstr);
            } catch(fs::filesystem_error& e) {
                Mess::DispMess(e);
                this->reject();
            } catch(std::bad_alloc& e) {
                Mess::DispMess(e);
                this->reject();
            } catch (std::exception& e) { // Not using fs::filesystem_error since std::bad_alloc can throw too.
                // Handle exception or use error code overload of fs::copy.
                Mess::DispMess(e);
                this->reject();
            }
        }
    }
    this->parent->show();
    this->close();
}

QString DialogPrompt::getPath() {
    return this->path;
}
