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
    ui->tableWidgetRight->setCurrentIndex(ui->tableWidgetRight->model()->index(0, 0));
    ui->tableWidgetLeft->setCurrentIndex(ui->tableWidgetLeft->model()->index(0, 0));
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
                if (this->getPath(side) != "/") {
                    entry->setValue(Type::Directory, lecture->d_name, 0, modif);
                }
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
    this->hash_side_entry.insert(side, vec_entry);
}

void Dialog::setDir(QString dir, QString side) {
    this->hash_path.insert(side, dir);
    if (side == "L") {
        ui->pathLeft->setText(dir);
    } else if (side == "R") {
        ui->pathRight->setText(dir);
    }
    this->setList(side);
}

QString Dialog::getPath(QString side) {
    return this->hash_path[side];
}

bool Dialog::EntryCompare::operator()(Entry *a, Entry *b) const {
    return(std::make_tuple(a->getOrder(), a->getName().toLower()) < std::make_tuple(b->getOrder(), b->getName().toLower()));
}

void Dialog::execCmd(QString cmd, QString side) {
    bool swFirstTime = true;
    begin:
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
                try {
                    if (word.endsWith("/")) {
                        this->setDir(word, side);
                    } else {
                        this->setDir(word + "/", side);
                    }
                } catch (ErrDirNotFound &e) {
                    // TODO Class/Method
                    if (swFirstTime) {
                        QMessageBox msgBox;
                        msgBox.setText(e.what());
                        msgBox.setInformativeText(e.description);
                        msgBox.setStandardButtons(QMessageBox::Ok);
                        msgBox.setDefaultButton(QMessageBox::Ok);
                        msgBox.setStyleSheet("QLabel{min-width:500 px; font-size: 24px;} QPushButton{ width:250px; font-size: 18px; }");
                        msgBox.exec();
                    }
                    QString dir = this->getPath(side);
                    QString lastIndex = "/";
                    int lastIndexOfSlash = dir.lastIndexOf(lastIndex);
                    if (dir.data()[0] != '/' || lastIndexOfSlash == -1 || lastIndexOfSlash == 0) {
                        this->setDir("/", side);
                    } else {
                        dir = this->minusOneLevel(dir);
                        cmd = "cd " + dir;
                        swFirstTime = false;
                        goto begin;
                    }
                }
            }
            if (side == "L") {
                ui->tableWidgetLeft->setFocus();
                ui->tableWidgetLeft->selectRow(0);
            } else {
                ui->tableWidgetRight->setFocus();
                ui->tableWidgetRight->selectRow(0);
            }
            break;
        }
        i++;
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
                    QVector<Entry*> vec_entry = this->hash_side_entry[side];
                    QString dir_enter = vec_entry.takeAt(fwtable->currentRow())->getName();
                    if (dir_enter == "..") {
                        QString lastIndex = "/";
                        int lastIndexOfSlash = dir.lastIndexOf(lastIndex);
                        if (dir.data()[0] != '/' || lastIndexOfSlash == -1 || lastIndexOfSlash == 0) {
                            this->setDir("/", side);
                        } else {
                            dir = this->minusOneLevel(dir);
                        }
                    } else {
                        dir += dir_enter;
                    }
                    if (dir == "") {
                        dir = "/";
                    }
                    this->setDir(dir, side);
                    this->execCmd("cd " + dir,side);
                }
            }
        } else {
            return QObject::eventFilter(obj, event);
        }
        return true;
    } else {
        return QObject::eventFilter(obj, event);
    }
    return false;
}

QString Dialog::minusOneLevel(QString dir) {
    QStringList list = dir.split('/');
    dir = "";
    for (int j = 0; j < (list.size() - 2); ++j) {
        if (j == 0) {
            dir = list[j];
        } else {
            dir += "/" + list[j];
        }
    }
    return dir;
}
void Dialog::on_tableWidgetLeft_cellDoubleClicked(int row, int column)
{
    QString side = "L";
    QString dir = ui->pathLeft->text();
    QVector<Entry*> vec_entry = this->hash_side_entry[side];
    QString dir_enter = vec_entry.takeAt(ui->tableWidgetLeft->currentRow())->getName();
    if (dir_enter == "..") {
        QString lastIndex = "/";
        int lastIndexOfSlash = dir.lastIndexOf(lastIndex);
        if (dir.data()[0] != '/' || lastIndexOfSlash == -1 || lastIndexOfSlash == 0) {
            this->setDir("/", side);
        } else {
            dir = this->minusOneLevel(dir);
        }
    } else {
        dir += dir_enter;
    }
    if (dir == "") {
        dir = "/";
    }
    this->setDir(dir, side);
    this->execCmd("cd " + dir,side);
}


void Dialog::on_tableWidgetRight_cellDoubleClicked(int row, int column)
{
    QString side = "R";
    QString dir = ui->pathRight->text();
    QVector<Entry*> vec_entry = this->hash_side_entry[side];
    QString dir_enter = vec_entry.takeAt(ui->tableWidgetRight->currentRow())->getName();
    if (dir_enter == "..") {
        QString lastIndex = "/";
        int lastIndexOfSlash = dir.lastIndexOf(lastIndex);
        if (dir.data()[0] != '/' || lastIndexOfSlash == -1 || lastIndexOfSlash == 0) {
            this->setDir("/", side);
        } else {
            dir = this->minusOneLevel(dir);
        }
    } else {
        dir += dir_enter;
    }
    if (dir == "") {
        dir = "/";
    }
    this->setDir(dir, side);
    this->execCmd("cd " + dir,side);
}
