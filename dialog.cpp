#include "dialog.h"
#include "./ui_dialog.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <QDebug>
#include <iostream>
#include <QDateTime>
#include "entry.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
    this->setRepertoire("/Users/stephane/", "L");
    this->setList("L");
}

Dialog::~Dialog()
{
    delete ui;
}


void Dialog::setList(QString side)
{
    QVector<Entry*> vec_entry;
    QString repertoire_courant = this->getRepertoire(side);
    struct dirent *lecture;
    DIR *dir;
    struct stat buf;
    QString currentPath;
    int row = 0;

    dir = opendir(repertoire_courant.toLocal8Bit()); // "." windows "C://"
    if (dir == NULL) {
        qCritical() << "Le répertoire " << repertoire_courant << " n'existe pas";
        reject();
    }

    while ((lecture = readdir(dir)) != NULL) {
        if (strcmp(lecture->d_name, ".") != 0) {
            currentPath = repertoire_courant + lecture->d_name;
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
        ui->tableWidgetGauche->setColumnCount(3);
        ui->tableWidgetGauche->setVerticalHeaderItem(0, new QTableWidgetItem("Nom"));
        ui->tableWidgetGauche->setVerticalHeaderItem(1, new QTableWidgetItem("Taille"));
        ui->tableWidgetGauche->setVerticalHeaderItem(2, new QTableWidgetItem("Date de modification"));
        int i = 0;
        foreach(Entry *v, vec_entry) {
            ui->tableWidgetGauche->insertRow(i);
            ui->tableWidgetGauche->setItem(i, 0, new QTableWidgetItem(v->getName(),Qt::DisplayRole));
            ui->tableWidgetGauche->setItem(i, 1, new QTableWidgetItem(v->getSizeString()));
            ui->tableWidgetGauche->setItem(i, 2, new QTableWidgetItem(v->getDateLastModifString()));
            i++;
        }
    } else if (side == "R") {
        ui->tableWidgetDroite->setColumnCount(3);
        ui->tableWidgetDroite->setVerticalHeaderItem(0, new QTableWidgetItem("Nom"));
        ui->tableWidgetDroite->setVerticalHeaderItem(1, new QTableWidgetItem("Taille"));
        ui->tableWidgetDroite->setVerticalHeaderItem(2, new QTableWidgetItem("Date de modification"));
        int i = 0;
        foreach(Entry *v, vec_entry) {
            ui->tableWidgetDroite->insertRow(i);
            ui->tableWidgetDroite->setItem(i, 0, new QTableWidgetItem(v->getName()));
            ui->tableWidgetDroite->setItem(i, 1, new QTableWidgetItem(v->getSizeString()));
            ui->tableWidgetDroite->setItem(i, 2, new QTableWidgetItem(v->getDateLastModifString()));
            i++;
        }
    }
}

void Dialog::setRepertoire(QString repertoire, QString side) {
    this->repertoire.insert(side, repertoire);
}

QString Dialog::getRepertoire(QString side) {
    // TODO security try catch
    return this->repertoire[side];
}

bool Dialog::EntryCompare::operator()(Entry *a, Entry *b) const {
    return(std::make_tuple(a->getSize(), a->getName()) < std::make_tuple(b->getSize(), b->getName())); // TODO case sensitive
}
