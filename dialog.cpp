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

    ls("/Users/stephane/");
    entry("/Users/stephane/");


}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::ls(QString repertoire)
{
    QVector<Entry*> vec_entry;
    struct dirent *lecture;
    DIR *dir;
    struct stat buf;
    QString currentPath;

    dir = opendir(repertoire.toLocal8Bit()); // "." windows "C://"
    if (dir == NULL) {
        qCritical() << "Le répertoire " << repertoire << " n'existe pas";
        reject();
    }

    int row = 0;
    ui->tableWidgetGauche->setColumnCount(3);
    ui->tableWidgetGauche->setVerticalHeaderItem(0, new QTableWidgetItem("Nom"));
    ui->tableWidgetGauche->setVerticalHeaderItem(1, new QTableWidgetItem("Taille"));
    ui->tableWidgetGauche->setVerticalHeaderItem(2, new QTableWidgetItem("Date de modification"));
    ui->tableWidgetGauche->setRowCount(countLs(repertoire));
    while ((lecture = readdir(dir)) != NULL) {
        if (strcmp(lecture->d_name, ".") != 0) {
            currentPath = repertoire + lecture->d_name;
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
            QString s_size = QString::number(size);
            QDateTime modif = QDateTime::fromSecsSinceEpoch(buf.st_mtime);
            if (!strcmp(lecture->d_name, "..")) {
                qInfo() << ".. is path";
                Entry *entry = new Entry();
                entry->setValue(Type::Directory, lecture->d_name, 0, modif);
                vec_entry.append(entry);
                ui->tableWidgetGauche->setItem(row, 0, new QTableWidgetItem(lecture->d_name));
                ui->tableWidgetGauche->setItem(row, 1, new QTableWidgetItem("REP"));
                ui->tableWidgetGauche->setItem(row, 2, new QTableWidgetItem(buf.st_mode));
            } else {
                qInfo() << lecture->d_name;
                if (S_ISDIR(buf.st_mode)) {
                    QString qstringTemp = lecture->d_name;
                    qstringTemp += "/";
                //vec_entry.append(new Entry(Type::Directory, lecture->d_name, 0, modif));
                //    current.set(Type::Directory, qstringTemp, 0, modif);
                    ui->tableWidgetGauche->setItem(row, 0, new QTableWidgetItem(qstringTemp));
                    ui->tableWidgetGauche->setItem(row, 1, new QTableWidgetItem("REP"));
                    ui->tableWidgetGauche->setItem(row, 2, new QTableWidgetItem(buf.st_mode));
                } else {
                  //  current.set(Type::File, lecture->d_name, 0, modif);
                    ui->tableWidgetGauche->setItem(row, 0, new QTableWidgetItem(lecture->d_name));
                    ui->tableWidgetGauche->setItem(row, 1, new QTableWidgetItem(s_size));
                    ui->tableWidgetGauche->setItem(row, 2, new QTableWidgetItem(modif.toString("dd.MM.yyyy hh:mm:ss")));
                }
            }
            row++;
        }
    }
    closedir(dir);
}

int Dialog::countLs(QString repertoire) {
    struct dirent *lecture;
    DIR *dir;
    QString currentPath;

    dir = opendir(repertoire.toLocal8Bit()); // "." windows "C://"
    if (dir == NULL) {
        qCritical() << "Le répertoire " << repertoire << " n'existe pas";
        reject();
    }

    int row = 0;
    while ((lecture = readdir(dir)) != NULL) {
        if (strcmp(lecture->d_name, ".") != 0) {
            row++;
        }
    }
    closedir(dir);
    return row;
}

void Dialog::entry(QString repertoire)
{
    struct dirent *lecture;
    DIR *dir;
    struct stat buf;
    QString currentPath;

    dir = opendir(repertoire.toLocal8Bit()); // "." windows "C://"
    if (dir == NULL) {
        qCritical() << "Le répertoire " << repertoire << " n'existe pas";
        reject();
    }

    while ((lecture = readdir(dir)) != NULL) {
        if (strcmp(lecture->d_name, ".") != 0) {
            currentPath = repertoire + lecture->d_name;
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
                    entry->setValue(Type::File, lecture->d_name, 0, modif);
                }
            }
        }
    }
    closedir(dir);
    /*foreach(Entry *v, vec_entry) {
        qInfo() << "Vector entry:" << v;
    }*/

}
