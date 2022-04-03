#include "dialog.h"
#include "./ui_dialog.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <QDebug>
#include <iostream>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ls("/Users/stephane/");
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::ls(QString repertoire)
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

    int row = 0;
    ui->tableWidgetGauche->setColumnCount(3);
    ui->tableWidgetGauche->setRowCount(countLs(repertoire));
    while ((lecture = readdir(dir)) != NULL) {
        if (strcmp(lecture->d_name, ".") != 0) {
            currentPath = repertoire + lecture->d_name;
            const char *charCurrentPath = currentPath.toLocal8Bit();
            int s = (stat(charCurrentPath, &buf));
            // page 864
            if (s == -1) {
                qCritical() << "stat" << currentPath;
            } else if (s) {
                buf.st_mode  = 0;
                buf.st_size = 0;
                buf.st_atime = 0;
                buf.st_mtime = 0;
                buf.st_ctime = 0;
            }
            int size = buf.st_size;
            QString s_size = QString::number(size);
            if (!strcmp(lecture->d_name, "..")) {
                qInfo() << ".. is path";
                ui->tableWidgetGauche->setItem(row, 0, new QTableWidgetItem(lecture->d_name));
                ui->tableWidgetGauche->setItem(row, 1, new QTableWidgetItem("REP"));
                ui->tableWidgetGauche->setItem(row, 2, new QTableWidgetItem(buf.st_mode));
            } else {
                qInfo() << lecture->d_name;
                if (S_ISDIR(buf.st_mode)) {
                    QString qstringTemp = lecture->d_name;
                    qstringTemp += "/";
                    ui->tableWidgetGauche->setItem(row, 0, new QTableWidgetItem(qstringTemp));
                    ui->tableWidgetGauche->setItem(row, 1, new QTableWidgetItem("REP"));
                    ui->tableWidgetGauche->setItem(row, 2, new QTableWidgetItem(buf.st_mode));
                } else {
                    ui->tableWidgetGauche->setItem(row, 0, new QTableWidgetItem(lecture->d_name));
                    ui->tableWidgetGauche->setItem(row, 1, new QTableWidgetItem(s_size));
                    ui->tableWidgetGauche->setItem(row, 2, new QTableWidgetItem(buf.st_mode));
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