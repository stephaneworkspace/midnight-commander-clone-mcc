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
    try {
        this->setDir("/Users/stephane/", "L");
    } catch (ErrDirNotFound &e) {
        // TODO Class/Method
        QMessageBox msgBox;
        msgBox.setText(e.what());
        msgBox.setInformativeText(e.description);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.setStyleSheet("QLabel{min-width:500 px; font-size: 24px;} QPushButton{ width:250px; font-size: 18px; }");
        msgBox.exec();
        this->setDir("/", "L");
    }
    try {
        this->setDir("/Users/stephane/Code/", "R");
    } catch (ErrDirNotFound &e) {
        // TODO Class/Method
        QMessageBox msgBox;
        msgBox.setText(e.what());
        msgBox.setInformativeText(e.description);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.setStyleSheet("QLabel{min-width:500 px; font-size: 24px;} QPushButton{ width:250px; font-size: 18px; }");
        msgBox.exec();
        this->setDir("/", "R");
    }
}

Dialog::~Dialog()
{
    delete ui;
}

/*
 * Err: ErrDirNotFound
 */
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
        ErrDirNotFound e;
        QString description = "Le répertoire " + path + " n'existe pas";
        e.description = description;
        throw e;
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
            } else {
                if (S_ISDIR(buf.st_mode)) {
                    QString qstringTemp = lecture->d_name;
                    qstringTemp += "/";
                    entry->setValue(Type::Directory, qstringTemp, 0, modif);
                } else if (S_ISBLK(buf.st_mode)) {
                    entry->setValue(Type::DeviceBloc, lecture->d_name, size, modif); // TODO test if size
                } else if(S_ISCHR(buf.st_mode)) {
                    entry->setValue(Type::DeviceCharacter, lecture->d_name, size, modif); // TODO test if size
                } else if(S_ISFIFO(buf.st_mode)) {
                    entry->setValue(Type::Fifo, lecture->d_name, size, modif); // TODO test if size
                } else if(S_ISLNK(buf.st_mode)) {
                    entry->setValue(Type::SymbolicLink, lecture->d_name, size, modif); // TODO test if size
                } else if(S_ISSOCK(buf.st_mode)) {
                    entry->setValue(Type::Socket, lecture->d_name, size, modif); // TODO test if size
                } else if(S_ISREG(buf.st_mode)) {
                    entry->setValue(Type::File, lecture->d_name, size, modif);
                } else {
                    entry->setValue(Type::Other, lecture->d_name, size, modif);
                }
            }
            vec_entry.append(entry);
            row++;
        }
    }
    delete lecture;
    closedir(dir);
    std::sort(vec_entry.begin(), vec_entry.end(), Dialog::EntryCompare());

    // Ui
    if (side == "L") {
        ui->pathLeft->setText(this->getPath(side));
        ui->pathLeft->adjustSize();
        ui->tableWidgetLeft->setColumnCount(3);
        ui->tableWidgetLeft->setHorizontalHeaderItem(0, new QTableWidgetItem("Nom"));
        ui->tableWidgetLeft->setHorizontalHeaderItem(1, new QTableWidgetItem("Taille"));
        ui->tableWidgetLeft->setHorizontalHeaderItem(2, new QTableWidgetItem("Date de modification"));
        int i = 0;
        foreach(Entry *v, vec_entry) {
            ui->tableWidgetLeft->insertRow(i);
            ui->tableWidgetLeft->setItem(i, 0, new QTableWidgetItem(v->getName(),Qt::DisplayRole));
                ui->tableWidgetLeft->setItem(i, 1, new QTableWidgetItem(v->getSizeString(2))); // TODO const
            ui->tableWidgetLeft->setItem(i, 2, new QTableWidgetItem(v->getDateLastChangeString()));
            i++;
        }
        ui->tableWidgetLeft->adjustSize();
        ui->tableWidgetLeft->resizeColumnsToContents();
    } else if (side == "R") {
        ui->pathRight->setText(this->getPath(side));
        ui->pathRight->adjustSize();
        ui->tableWidgetRight->setColumnCount(3);
        ui->tableWidgetRight->setHorizontalHeaderItem(0, new QTableWidgetItem("Nom"));
        ui->tableWidgetRight->setHorizontalHeaderItem(1, new QTableWidgetItem("Taille"));
        ui->tableWidgetRight->setHorizontalHeaderItem(2, new QTableWidgetItem("Date de modification"));
        int i = 0;
        foreach(Entry *v, vec_entry) {
            ui->tableWidgetRight->insertRow(i);
            ui->tableWidgetRight->setItem(i, 0, new QTableWidgetItem(v->getName()));
            ui->tableWidgetRight->setItem(i, 1, new QTableWidgetItem(v->getSizeString(2))); // TODO const
            ui->tableWidgetRight->setItem(i, 2, new QTableWidgetItem(v->getDateLastChangeString()));
            i++;
        }
        ui->tableWidgetRight->adjustSize();
        ui->tableWidgetRight->resizeColumnsToContents();
    }
}

void Dialog::setDir(QString dir, QString side) {
    this->hash_path.insert(side, dir);
    this->setList(side);
}

QString Dialog::getPath(QString side) {
    return this->hash_path[side];
}

bool Dialog::EntryCompare::operator()(Entry *a, Entry *b) const {
    return(std::make_tuple(a->getOrder(), a->getName().toLower()) < std::make_tuple(b->getOrder(), b->getName().toLower()));
}

void Dialog::on_lineEditCmdLeft_returnPressed() {
   execCmd(ui->lineEditCmdLeft->text(), "L");
   ui->lineEditCmdLeft->clear();
}

void Dialog::on_lineEditCmdRight_returnPressed() {
    execCmd(ui->lineEditCmdRight->text(), "R");
    ui->lineEditCmdRight->clear();
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
                try {
                    if (word.endsWith("/")) {
                        this->setDir(word, side);
                    } else {
                        this->setDir(word + "/", side);
                    }
                } catch (ErrDirNotFound &e) {
                    // TODO Class/Method
                    QMessageBox msgBox;
                    msgBox.setText(e.what());
                    msgBox.setInformativeText(e.description);
                    msgBox.setStandardButtons(QMessageBox::Ok);
                    msgBox.setDefaultButton(QMessageBox::Ok);
                    msgBox.setStyleSheet("QLabel{min-width:500 px; font-size: 24px;} QPushButton{ width:250px; font-size: 18px; }");
                    msgBox.exec();
                    this->setDir("/", side); // TODO manipulation QString pour venir / en arrière et si == " " -> /
                }
            }
            // TODO curseur sur QWidgetTable side
            break;
        }
        i++;
    }
}


void Dialog::on_pushButtonF1_clicked()
{
   //this->reject(); TODO conflit avec execCmd ???
}

