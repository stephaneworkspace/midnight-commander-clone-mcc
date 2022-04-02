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

    while ((lecture = readdir(dir)) != NULL) {
        if (strcmp(lecture->d_name, ".") != 0) {
            if (!strcmp(lecture->d_name, "..")) {
                qInfo() << ".. is path";
                ui->listWidgetGauche->addItem(lecture->d_name);
            } else {
                currentPath = repertoire + lecture->d_name;
                const char *charCurrentPath = currentPath.toLocal8Bit();
                if ((stat(charCurrentPath, &buf)) == -1) {
                    qCritical() << "stat" << currentPath;
                }
                if (S_ISDIR(buf.st_mode)) {
                    QString qstringTemp = lecture->d_name;
                    qstringTemp += "/";
                    ui->listWidgetGauche->addItem(qstringTemp);
                } else {
                    ui->listWidgetGauche->addItem(lecture->d_name);
                }
            }
        }
    }
    closedir(dir);
}

