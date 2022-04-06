#include "dialog.h"
#include "./ui_dialog.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <QDebug>
#include <iostream>
#include <QDateTime>
#include <QMessageBox>
#include "entry.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
    this->setDir("/Users/stephane/", "L");
    this->setDir("/Users/stephane/Code/", "R");
    this->setList("L");
    this->setList("R");
}

Dialog::~Dialog()
{
    delete ui;
}


void Dialog::setList(QString side)
{
    if (side == "L") {
        ui->tableWidgetLeft->clear();
        ui->tableWidgetLeft->setRowCount(0);
    } else {
        ui->tableWidgetRight->clear();
        ui->tableWidgetRight->setRowCount(0);
    }
    QVector<Entry*> vec_entry;
    QString path = this->getPath(side);
    struct dirent *lecture;
    DIR *dir;
    struct stat buf;
    QString currentPath;
    int row = 0;

    dir = opendir(path.toLocal8Bit()); // "." windows "C://"
    if (dir == NULL) {
        qCritical() << "Le répertoire " << path << " n'existe pas";
        //reject(); // TODO throw

        QMessageBox msgBox;
        msgBox.setText("Path invalid");
        msgBox.setInformativeText("Your path \"" + path + "\" is invalid");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        goto end; // TODO cursor on path
    }

    while ((lecture = readdir(dir)) != NULL) {
        if (strcmp(lecture->d_name, ".") != 0) {
            currentPath = path + lecture->d_name;
            const char *charCurrentPath = currentPath.toLocal8Bit();
            if ((stat(charCurrentPath, &buf)) == -1) {
                qCritical() << "stat" << currentPath;
                /*
                struct stat {
                    dev_t st_dev; // ID of device containing file
                    ino_t st_ino; // inode number
                    mode_t st_mode; // protection
                    nlink_t st_nlink; // number of hard links
                    uid_t st_uid; // user ID of owner
                    gid_t st_gid; // group ID of owner
                    dev_t st_rdev; // device ID (if special file)
                    off_t st_size; // total size, in bytes
                    blksize_t st_blksize; // blocksize for file system I/O
                    blkcnt_t st_blocks; // number of 512B blocks allocated
                    time_t st_atime; // time of last access
                    time_t st_mtime; // time of last modification
                    time_t st_ctime; // time of last status change
                };*/
            }
            int size = buf.st_size;
            QDateTime modif = QDateTime::fromSecsSinceEpoch(buf.st_mtime);
            Entry *entry = new Entry();
            if (!strcmp(lecture->d_name, "..")) {
                entry->setValue(Type::Directory, lecture->d_name, 0, modif);
                vec_entry.append(entry);
            } else {
                vec_entry.append(entry);
                if (S_ISDIR(buf.st_mode)) {
                    QString qstringTemp = lecture->d_name;
                    qstringTemp += "/";
                    entry->setValue(Type::Directory, qstringTemp, 0, modif);
                } else {
                    entry->setValue(Type::File, lecture->d_name, size, modif);
                } // TODO complete all kind of posibility in unix
            }
            row++;
        }
    }
    closedir(dir);

    std::sort(vec_entry.begin(), vec_entry.end(), Dialog::EntryCompare());

    // Ui
    if (side == "L") {
        ui->pathLeft->setText(this->getPath(side));
        ui->pathLeft->adjustSize();
        ui->tableWidgetLeft->setColumnCount(3);
        ui->tableWidgetLeft->setVerticalHeaderItem(0, new QTableWidgetItem("Nom"));
        ui->tableWidgetLeft->setVerticalHeaderItem(1, new QTableWidgetItem("Taille"));
        ui->tableWidgetLeft->setVerticalHeaderItem(2, new QTableWidgetItem("Date de modification"));
        int i = 0;
        foreach(Entry *v, vec_entry) {
            ui->tableWidgetLeft->insertRow(i);
            ui->tableWidgetLeft->setItem(i, 0, new QTableWidgetItem(v->getName(),Qt::DisplayRole));
            ui->tableWidgetLeft->setItem(i, 1, new QTableWidgetItem(v->getSizeString()));
            ui->tableWidgetLeft->setItem(i, 2, new QTableWidgetItem(v->getDateLastModifString()));
            i++;
        }
    } else if (side == "R") {
        ui->pathRight->setText(this->getPath(side));
        ui->pathRight->adjustSize();
        ui->tableWidgetRight->setColumnCount(3);
        ui->tableWidgetRight->setVerticalHeaderItem(0, new QTableWidgetItem("Nom"));
        ui->tableWidgetRight->setVerticalHeaderItem(1, new QTableWidgetItem("Taille"));
        ui->tableWidgetRight->setVerticalHeaderItem(2, new QTableWidgetItem("Date de modification"));
        int i = 0;
        foreach(Entry *v, vec_entry) {
            ui->tableWidgetRight->insertRow(i);
            ui->tableWidgetRight->setItem(i, 0, new QTableWidgetItem(v->getName()));
            ui->tableWidgetRight->setItem(i, 1, new QTableWidgetItem(v->getSizeString()));
            ui->tableWidgetRight->setItem(i, 2, new QTableWidgetItem(v->getDateLastModifString()));
            i++;
        }
    }
    end:
    {}
}

void Dialog::setDir(QString dir, QString side) {
    this->hash_path.insert(side, dir);
}

QString Dialog::getPath(QString side) {
    return this->hash_path[side];
}

bool Dialog::EntryCompare::operator()(Entry *a, Entry *b) const {
    return(std::make_tuple(a->getOrder(), a->getName().toLower()) < std::make_tuple(b->getOrder(), b->getName().toLower()));
}

void Dialog::on_lineEditCmdLeft_returnPressed() {
   execCmd(ui->lineEditCmdLeft->text(), "L");
}

void Dialog::on_lineEditCmdRight_returnPressed() {
    execCmd(ui->lineEditCmdRight->text(), "R");
}

void Dialog::execCmd(QString cmd, QString side) {
    QStringList words_list = cmd.split(QLatin1Char(' '));
    int i = 0;
    Cmd c = Cmd::None;
    foreach(const QString &word, words_list) {
        if (i == 0) {
            if (word == "cd") {
                c = Cmd::cd;
            }
        } else {
            if (c == Cmd::cd) {
                // TODO ..
                // TODO ajouter / à la fin
                this->setDir(word, side);
                this->setList(side);
            }
            break;
        }
        i++;
    }
}

