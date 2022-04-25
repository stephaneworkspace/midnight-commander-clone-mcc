#include "dialog.h"
#include "./ui_dialog.h"
#include <sys/types.h>
#include <QDebug>
#include <QMessageBox>
#include <QMenu>
#include <QMenuBar>
#include <QKeyEvent>
#include <QProcess>
#include "mess.h"
#include "entry.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
    this->sideFocus = "L";

    // Detect dark mode
    QColor color = QPalette().color(QPalette::Window).value();
    QColor colorText = QPalette().color(QPalette::WindowText).value();
    this->swDarkMode = colorText.value() > color.value();

    // Menu
    QMenu *i1 = new QMenu("Fichier");
    i1->addAction("cHmod");
    i1->addAction("Lien");
    i1->addAction("Lien symbolique");
    i1->addAction("Editer lien symbolique");
    i1->addAction("ch0wn");
    i1->addAction("ch0wn Avancé");
    QMenu *i2 = new QMenu("Commande");
    i2->addAction("TODO");
    QMenu *i3 = new QMenu("Options");
    i3->addAction("TODO");
    QMenuBar *t = new QMenuBar;
    t->addMenu(i1);
    t->addMenu(i2);
    t->addMenu(i3);
    t->show();

    // Event key press
    this->installEventFilter(this);

    // Left - Right QTableWidget
    if (swDarkMode) {
        QPalette paletteL = ui->tableWidgetLeft->palette();
        paletteL.setBrush(QPalette::Highlight,QBrush(Qt::white));
        paletteL.setBrush(QPalette::HighlightedText,QBrush(Qt::black));
        ui->tableWidgetLeft->setPalette(paletteL);
        QPalette paletteR = ui->tableWidgetRight->palette();
        paletteR.setBrush(QPalette::Highlight,QBrush(Qt::white));
        paletteR.setBrush(QPalette::HighlightedText,QBrush(Qt::black));
        ui->tableWidgetRight->setPalette(paletteR);
    }

    this->entrys = new Entrys();
    try {
        this->entrys->setDir("/Users/stephane/", "L");
    } catch (ErrDirNotFound &e) {
        Mess::DispMess(e);
        this->entrys->setDir("/", "L");
    }
    try {
        this->entrys->setDir("/Users/stephane/Code/", "R");
    } catch (ErrDirNotFound &e) {
        Mess::DispMess(e);
        this->entrys->setDir("/", "R");
    }

    this->setListUi("R");
    this->setListUi("L");

    ui->tableWidgetRight->setCurrentIndex(ui->tableWidgetRight->model()->index(0, 0));
    ui->tableWidgetLeft->setCurrentIndex(ui->tableWidgetLeft->model()->index(0, 0));
}

Dialog::~Dialog()
{
    delete ui;
    delete dialogPrompt;
}

void Dialog::execCmd(QString cmd, QString side) {
    this->entrys->execCmd(cmd, side);
    this->setListUi(side);
}

bool Dialog::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* key = static_cast<QKeyEvent*>(event);
        if ( (key->key() == Qt::Key_Enter) || (key->key() == Qt::Key_Return)) {
            QWidget* fw = this->focusWidget();
            if (fw != Q_NULLPTR) {
                if (fw->objectName() == "lineEditCmdLeft") {
                    execCmd(ui->lineEditCmdLeft->text(), "L");
                    ui->lineEditCmdLeft->clear();
                    this->sideFocus = "L";
                } else if (fw->objectName() == "lineEditCmdRight") {
                    execCmd(ui->lineEditCmdRight->text(), "R");
                    ui->lineEditCmdRight->clear();
                    this->sideFocus = "R";
                } else if (fw->objectName() == "tableWidgetLeft" || fw->objectName() == "tableWidgetRight") {
                    QTableWidget *fwtable = dynamic_cast<QTableWidget *>(this->focusWidget());
                    QString side;
                    QString dir;
                    if (fw->objectName() == "tableWidgetLeft") {
                        side = "L";
                        dir = ui->pathLeft->text();
                    } else if (fw->objectName() == "tableWidgetRight") {
                        side = "R";
                        dir = ui->pathRight->text();
                    }
                    this->entrys->setDir(dir, side);
                    int row = fwtable->currentRow();
                    QString key = fwtable->item(row,0)->text();
                    Entry* entry = this->entrys->getEntry(side, key);
                    if (entry == nullptr) {
                        dir = "/";
                        this->entrys->setDir(dir, side);
                        this->setListUi(side);
                    } else {
                        QString entryName = entry->getName();
                        Type entryType = entry->getType();
                        if (entryType == Type::File) {
                            QProcess *process = new QProcess(this);
                            QString program = "/usr/bin/open";
                            QStringList args;
                            args << dir + entryName;
                            process->setProgram(program);
                            process->setArguments(args);
                            process->start();
                        } else if (entryType == Type::Directory) {
                            if (entryName == "..") {
                                QString lastIndex = "/";
                                int lastIndexOfSlash = dir.lastIndexOf(lastIndex);
                                if (dir.data()[0] != '/' || lastIndexOfSlash == -1 || lastIndexOfSlash == 0) {
                                    this->entrys->setDir("/", side);
                                } else {
                                    dir = this->entrys->minusOneLevel(dir);
                                }
                            } else {
                                dir += entryName;
                            }
                            if (dir == "") {
                                dir = "/";
                            }
                            this->entrys->setDir(dir, side);
                            this->setListUi(side);
                        }
                    }
                }
            }
        } else {
            return QObject::eventFilter(obj, event);
        }
        return true;
    } else {
        return QObject::eventFilter(obj, event);
    }
}

void Dialog::on_tableWidgetLeft_cellDoubleClicked(int row, int column)
{
    QString side = "L";
    QString dir = ui->pathLeft->text();
    QString key = ui->tableWidgetLeft->item(row,0)->text();
    this->cellClick(side, dir, key);
    this->sideFocus = "L";
}


void Dialog::on_tableWidgetRight_cellDoubleClicked(int row, int column)
{
    QString side = "R";
    QString dir = ui->pathRight->text();
    QString key = ui->tableWidgetRight->item(row,0)->text();
    this->cellClick(side, dir, key);
    this->sideFocus = "R";
}

void Dialog::setListUi(QString side) {
    QString dir = this->entrys->getPath(side);
    if (side == "L") {
        ui->pathLeft->setText(dir);
        ui->pathLeft->adjustSize();
        ui->tableWidgetLeft->clear();
        ui->tableWidgetLeft->setRowCount(0);
        ui->tableWidgetLeft->setColumnCount(4);
        ui->tableWidgetLeft->setHorizontalHeaderItem(0, new QTableWidgetItem("Nom"));
        ui->tableWidgetLeft->setHorizontalHeaderItem(1, new QTableWidgetItem("Taille"));
        ui->tableWidgetLeft->setHorizontalHeaderItem(2, new QTableWidgetItem("Date de modification"));
        ui->tableWidgetLeft->setHorizontalHeaderItem(3, new QTableWidgetItem("Permissions"));
        int i = 0;
            foreach(Entry *v, this->entrys->getEntrys(side)) {
                ui->tableWidgetLeft->insertRow(i);
                ui->tableWidgetLeft->setItem(i, 0, new QTableWidgetItem(v->getName(),Qt::DisplayRole));
                ui->tableWidgetLeft->setItem(i, 1, new QTableWidgetItem(v->getSizeString(2)));
                ui->tableWidgetLeft->setItem(i, 2, new QTableWidgetItem(v->getDateLastChangeString()));
                ui->tableWidgetLeft->setItem(i, 3, new QTableWidgetItem(v->getMode()));
                i++;
            }
        //ui->tableWidgetLeft->adjustSize();
        ui->tableWidgetLeft->resizeColumnsToContents();
    } else if (side == "R") {
        ui->pathRight->setText(dir);
        ui->pathRight->adjustSize();
        ui->tableWidgetRight->clear();
        ui->tableWidgetRight->setRowCount(0);
        ui->tableWidgetRight->setColumnCount(4);
        ui->tableWidgetRight->setHorizontalHeaderItem(0, new QTableWidgetItem("Nom"));
        ui->tableWidgetRight->setHorizontalHeaderItem(1, new QTableWidgetItem("Taille"));
        ui->tableWidgetRight->setHorizontalHeaderItem(2, new QTableWidgetItem("Date de modification"));
        ui->tableWidgetRight->setHorizontalHeaderItem(3, new QTableWidgetItem("Permissions"));
        int i = 0;
            foreach(Entry *v, this->entrys->getEntrys(side)) {
                ui->tableWidgetRight->insertRow(i);
                ui->tableWidgetRight->setItem(i, 0, new QTableWidgetItem(v->getName()));
                ui->tableWidgetRight->setItem(i, 1, new QTableWidgetItem(v->getSizeString(2)));
                ui->tableWidgetRight->setItem(i, 2, new QTableWidgetItem(v->getDateLastChangeString()));
                ui->tableWidgetRight->setItem(i, 3, new QTableWidgetItem(v->getMode()));
                i++;
            }
        //ui->tableWidgetRight->adjustSize();
        ui->tableWidgetRight->resizeColumnsToContents();
    }
    if (side == "L") {
        ui->tableWidgetLeft->setFocus();
        ui->tableWidgetLeft->selectRow(0); // TODO rename
        // TODO cursor wheel to selectRow 0 if F7
    } else {
        ui->tableWidgetRight->setFocus();
        ui->tableWidgetRight->selectRow(0);
    }
}

void Dialog::cellClick(QString side, QString dir, QString key) {
    this->entrys->setDir(dir, side);
    Entry* entry = this->entrys->getEntry(side, key);
    if (entry == nullptr) {
        dir = "/";
        this->entrys->setDir(dir, side);
        this->setListUi(side);
    } else {
        QString entryName = entry->getName();
        Type entryType = entry->getType();
        if (entryType == Type::File) {
            QProcess *process = new QProcess(this);
            QString program = "/usr/bin/open";
            QStringList args;
            args << dir + entryName;
            process->setProgram(program);
            process->setArguments(args);
            process->start();
        } else if (entryType == Type::Directory) {
            if (entryName == "..") {
                QString lastIndex = "/";
                int lastIndexOfSlash = dir.lastIndexOf(lastIndex);
                if (dir.data()[0] != '/' || lastIndexOfSlash == -1 || lastIndexOfSlash == 0) {
                    this->entrys->setDir("/", side);
                } else {
                    dir = this->entrys->minusOneLevel(dir);
                }
            } else {
                dir += entryName;
            }
            if (dir == "") {
                dir = "/";
            }
            this->entrys->setDir(dir, side);
            this->setListUi(side);
        }
    }
}

void Dialog::on_pushButton_F7_clicked()
{
    this->hide();
    QString side = this->sideFocus;
    dialogPrompt = new DialogPrompt(this, this->entrys->getPath(side), Prompt::F7);
    dialogPrompt->show();
    dialogPrompt->exec();
    // TODO tester si reject
    QString path = dialogPrompt->getPath();
    if (path != "") {
        if (!path.endsWith("/")) {
            path = path + "/";
        }
        this->entrys->setDir(path, side); // TODO param option F7 goto dir
        this->setListUi(side); // TODO deuxième côté peut aussi être affecter avec reposition sur sélection
    }
}

void Dialog::on_tableWidgetLeft_cellClicked(int row, int column)
{
   this->sideFocus = "L";
}


void Dialog::on_tableWidgetRight_cellClicked(int row, int column)
{
   this->sideFocus = "R";
}


void Dialog::on_tableWidgetLeft_cellEntered(int row, int column)
{
   this->sideFocus = "L";
}


void Dialog::on_tableWidgetRight_cellEntered(int row, int column)
{
   this->sideFocus = "R";
}

/*
 * F6 Déplacer
 */
void Dialog::on_pushButton_F6_clicked()
{
    QString side = this->sideFocus;
    int row = side == "L" ? ui->tableWidgetLeft->currentRow() : ui->tableWidgetRight->currentRow();
    QString key = side == "L" ? ui->tableWidgetLeft->item(row, 0)->text() : ui->tableWidgetRight->item(row, 0)->text();
    this->entrys->setDir(side == "L" ? ui->pathLeft->text() : ui->pathRight->text(), side); // TODO bypass
    try {
        Entry* entry = this->entrys->getEntry(side, key); // TODO key not found
        if (entry == nullptr) {
            ErrKeyNotFound e;
            QString description = "L'objet " + key + " n'existe plus";
            e.description = description;
            throw e;
        }
        if (entry->getType() == Type::Directory || entry->getType() == Type::File) {
            // Supported
        } else {
            if (entry->getType() == Type::DeviceBloc) {
                Mess::DispMessQString("F6 Renomer/Déplacer","Device Bloc n'est pas supporté");
            } else if (entry->getType() == Type::DeviceCharacter) {
                Mess::DispMessQString("F6 Renomer/Déplacer","Device Character n'est pas supporté");
            } else if (entry->getType() == Type::Fifo) {
                Mess::DispMessQString("F6 Renomer/Déplacer","Fifo n'est pas supporté");
            } else if (entry->getType() == Type::Socket) {
                Mess::DispMessQString("F6 Renomer/Déplacer","Socket n'est pas supporté");
            } else if (entry->getType() == Type::SymbolicLink) {
                Mess::DispMessQString("F6 Renomer/Déplacer","Lien symbolique n'est pas supporté");
            } else {
                Mess::DispMessQString("F6 Renomer/Déplacer","N'est pas supporté");
            }
            return;
        }
    } catch (ErrKeyNotFound& e) { // Not using fs::filesystem_error since std::bad_alloc can throw too.
        // Handle exception or use error code overload of fs::copy.
        Mess::DispMess(e);
        return;
    }

    this->hide();
    //QString side = this->sideFocus;
    QString rename = "";
    QString sideOrigin = side;
    QString keyOrigin = "";
    if (side == "L") {
        int row = ui->tableWidgetLeft->currentRow();
        rename = ui->tableWidgetLeft->item(row,0)->text();
        side = "R";
    } else if (side == "R") {
        int row = ui->tableWidgetRight->currentRow();
        rename = ui->tableWidgetRight->item(row,0)->text();
        side = "L";
    }
    if (rename != "") {
        dialogPrompt = new DialogPrompt(this, this->entrys->getPath(side), Prompt::F6, rename, this->entrys->getPath(sideOrigin));
        dialogPrompt->show();
        dialogPrompt->exec();
        QString path = dialogPrompt->getPath();
        if (path != "") {
            // Ce code ne semble inutile c'est du copier coller F7 Créer
            if (!path.endsWith("/")) {
                path = path + "/";
            }
            this->entrys->setDir(side == "L" ? ui->pathRight->text() : ui->pathLeft->text(), side == "L" ? "R" : "L"); // Other side
            this->setListUi(side == "L" ? "R" : "L");
            this->entrys->setDir(path, side); // Other side
            this->setListUi(side);
        }
    }
}

/*
 * F5 Copier
 */
void Dialog::on_pushButton_F5_clicked()
{
   // Copier
   //QString side = this->sideFocus;
   if (this->entrys->getPath("L") == this->entrys->getPath("R")) {
       // Ne pas se copier sur sois même
       int row_L = ui->tableWidgetLeft->currentRow();
       QString key_L = ui->tableWidgetLeft->item(row_L,0)->text();
       int row_R = ui->tableWidgetRight->currentRow();
       QString key_R = ui->tableWidgetRight->item(row_R,0)->text();
       if (key_L == key_R) {
           Mess::DispMessQString("Copier","Ne peut pas se copier sur sois même");
           return;
       } else {
           QString side = this->sideFocus;
           if (side == "L") {
               for (int i = 0; i < ui->tableWidgetRight->rowCount(); i++) {
                   QString key_R = ui->tableWidgetRight->item(i,0)->text();
                   if (key_L == key_R) {
                       Mess::DispMessQString("Copier","Ne peut pas se copier sur sois même");
                       return;
                   }
               }
           } else if (side == "R") {
               for (int i = 0; i < ui->tableWidgetLeft->rowCount(); i++) {
                   QString key_R = ui->tableWidgetLeft->item(i,0)->text();
                   if (key_L == key_R) {
                       Mess::DispMessQString("Copier","Ne peut pas se copier sur sois même");
                       return;
                   }
               }
           }
           try {
               if (side == "L") {
                   QString copy_L = ui->pathLeft->text() + key_L;
                   QByteArray c_L_ba = copy_L.toLocal8Bit();
                   const char *c_L = c_L_ba.constData();
                   QString copy_R = ui->pathRight->text() + key_L;
                   QByteArray c_R_ba = copy_R.toLocal8Bit();
                   const char *c_R = c_R_ba.constData();
                   fs::copy(c_L, c_R, fs::copy_options::recursive /*| fs::copy_options::overwrite_existing*/);
                   this->entrys->setDir(ui->pathRight->text(), "R");
                   this->setListUi("R");
               } else if (side == "R") {
                   QString copy_L = ui->pathLeft->text() + key_R;
                   QByteArray c_L_ba = copy_L.toLocal8Bit();
                   const char *c_L = c_L_ba.constData();
                   QString copy_R = ui->pathRight->text() + key_R;
                   QByteArray c_R_ba = copy_R.toLocal8Bit();
                   const char *c_R = c_R_ba.constData();
                   fs::copy(c_R, c_L, fs::copy_options::recursive /*| fs::copy_options::overwrite_existing*/);
                   this->entrys->setDir(ui->pathLeft->text(), "L");
                   this->setListUi("L");
               }
           } catch(fs::filesystem_error& e) {
               Mess::DispMess(e);
           } catch(std::bad_alloc& e) {
               Mess::DispMess(e);
           } catch (std::exception& e) { // Not using fs::filesystem_error since std::bad_alloc can throw too.
               // Handle exception or use error code overload of fs::copy.
               Mess::DispMess(e);
           }
       }
   } else {
       // TODO tester recurssion infinie si ça existe (surtout pour F6 déplacer)
       int row_L = ui->tableWidgetLeft->currentRow();
       QString key_L = ui->tableWidgetLeft->item(row_L,0)->text();
       int row_R = ui->tableWidgetRight->currentRow();
       QString key_R = ui->tableWidgetRight->item(row_R,0)->text();
       QString side = this->sideFocus;
       if (side == "L") {
           for (int i = 0; i < ui->tableWidgetRight->rowCount(); i++) {
               QString key_R = ui->tableWidgetRight->item(i,0)->text();
               if (key_L == key_R) {
                   Mess::DispMessQString("Copier","Existe déjà"); // TODO faire mieux
                   return;
               }
           }
       } else if (side == "R") {
           for (int i = 0; i < ui->tableWidgetLeft->rowCount(); i++) {
               QString key_R = ui->tableWidgetLeft->item(i,0)->text();
               if (key_L == key_R) {
                   Mess::DispMessQString("Copier","Existe déjà"); // TODO faire mieux
                   return;
               }
           }
       }
       try {
           if (side == "L") {
               QString copy_L = ui->pathLeft->text() + key_L;
               QByteArray c_L_ba = copy_L.toLocal8Bit();
               const char *c_L = c_L_ba.constData();
               QString copy_R = ui->pathRight->text() + key_L;
               QByteArray c_R_ba = copy_R.toLocal8Bit();
               const char *c_R = c_R_ba.constData();
               fs::copy(c_L, c_R, fs::copy_options::recursive /*| fs::copy_options::overwrite_existing*/);
               this->entrys->setDir(ui->pathRight->text(), "R");
               this->setListUi("R");
           } else if (side == "R") {
               QString copy_L = ui->pathLeft->text() + key_R;
               QByteArray c_L_ba = copy_L.toLocal8Bit();
               const char *c_L = c_L_ba.constData();
               QString copy_R = ui->pathRight->text() + key_R;
               QByteArray c_R_ba = copy_R.toLocal8Bit();
               const char *c_R = c_R_ba.constData();
               fs::copy(c_R, c_L, fs::copy_options::recursive /*| fs::copy_options::overwrite_existing*/);
               this->entrys->setDir(ui->pathLeft->text(), "L");
               this->setListUi("L");
           }
       } catch(fs::filesystem_error& e) {
           Mess::DispMess(e);
       } catch(std::bad_alloc& e) {
           Mess::DispMess(e);
       } catch (std::exception& e) { // Not using fs::filesystem_error since std::bad_alloc can throw too.
           // Handle exception or use error code overload of fs::copy.
           Mess::DispMess(e);
       }
   }
}

