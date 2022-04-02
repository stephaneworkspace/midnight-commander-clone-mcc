#include "dialog.h"
#include "./ui_dialog.h"
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ls("/Users/stephane/Code/");
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::ls(QString repertoire)
{
    struct dirent *lecture;
    DIR *rep;
    rep = opendir(repertoire.toLatin1()); // "." windows "C://"

    while ((lecture = readdir(rep)) != NULL) {
        if ((strcmp(lecture->d_name, ".") != 0)) {
            printf("%s\n", lecture->d_name);
            ui->listWidgetGauche->addItem(lecture->d_name);
        }
    }

    closedir(rep);

}

