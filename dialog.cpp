#include "dialog.h"
#include "./ui_dialog.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <QDebug>
#include <iostream>
#include <QDateTime>
#include <QMessageBox>
#include <QMenu>
#include <QMenuBar>
#include <QKeyEvent>
#include <QProcess>
#include "entry.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);

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
    QPalette paletteL = ui->tableWidgetLeft->palette();
    paletteL.setBrush(QPalette::Highlight,QBrush(Qt::white));
    paletteL.setBrush(QPalette::HighlightedText,QBrush(Qt::black));
    ui->tableWidgetLeft->setPalette(paletteL);
    QPalette paletteR = ui->tableWidgetRight->palette();
    paletteR.setBrush(QPalette::Highlight,QBrush(Qt::white));
    paletteR.setBrush(QPalette::HighlightedText,QBrush(Qt::black));
    ui->tableWidgetRight->setPalette(paletteR);

    this->entrys = new Entrys();

    try {
        this->entrys->setDir("/Users/stephane/", "L");
    } catch (ErrDirNotFound &e) {
        // TODO Class/Method
        QMessageBox msgBox;
        msgBox.setText(e.what());
        msgBox.setInformativeText(e.description);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.setStyleSheet("QLabel{min-width:500 px; font-size: 24px;} QPushButton{ width:250px; font-size: 18px; }");
        msgBox.exec();
        this->entrys->setDir("/", "L");
    }
    try {
        this->entrys->setDir("/Users/stephane/Code/", "R");
    } catch (ErrDirNotFound &e) {
        // TODO Class/Method
        QMessageBox msgBox;
        msgBox.setText(e.what());
        msgBox.setInformativeText(e.description);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.setStyleSheet("QLabel{min-width:500 px; font-size: 24px;} QPushButton{ width:250px; font-size: 18px; }");
        msgBox.exec();
        this->entrys->setDir("/", "R");
    }
    this->setListUi("L");
    this->setListUi("R");

    ui->tableWidgetRight->setCurrentIndex(ui->tableWidgetRight->model()->index(0, 0));
    ui->tableWidgetLeft->setCurrentIndex(ui->tableWidgetLeft->model()->index(0, 0));
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::execCmd(QString cmd, QString side) {
    this->entrys->execCmd(cmd, side);
    this->setListUi(side);
    if (side == "L") {
        ui->tableWidgetLeft->setFocus();
        ui->tableWidgetLeft->selectRow(0);
    } else {
        ui->tableWidgetRight->setFocus();
        ui->tableWidgetRight->selectRow(0);
    }
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
                } else if (fw->objectName() == "lineEditCmdRight") {
                    execCmd(ui->lineEditCmdRight->text(), "L");
                    ui->lineEditCmdRight->clear();
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
    this->entrys->setDir(dir, side);
    QString key = ui->tableWidgetLeft->item(row,0)->text();
    this->cellClick(side, dir, key);
}


void Dialog::on_tableWidgetRight_cellDoubleClicked(int row, int column)
{
    QString side = "R";
    QString dir = ui->pathRight->text();
    this->entrys->setDir(dir, side);
    QString key = ui->tableWidgetRight->item(row,0)->text();
    this->cellClick(side, dir, key);
}

void Dialog::setListUi(QString side) {
    QString dir = this->entrys->getPath(side);
    if (side == "L") {
        ui->pathLeft->setText(dir);
    } else if (side == "R") {
        ui->pathRight->setText(dir);
    }
    if (side == "L") {
        ui->tableWidgetLeft->clear();
        ui->tableWidgetLeft->setRowCount(0);
    } else {
        ui->tableWidgetRight->clear();
        ui->tableWidgetRight->setRowCount(0);
    }
    // Ui
    if (side == "L") {
        ui->pathLeft->setText(dir);
        ui->pathLeft->adjustSize();
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
        ui->tableWidgetLeft->adjustSize();
        ui->tableWidgetLeft->resizeColumnsToContents();
    } else if (side == "R") {
        ui->pathRight->setText(dir);
        ui->pathRight->adjustSize();
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
        ui->tableWidgetRight->adjustSize();
        ui->tableWidgetRight->resizeColumnsToContents();
    }
}

void Dialog::cellClick(QString side, QString dir, QString key) {
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
