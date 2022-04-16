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
#include "entrys.h"
#include "mess.h"
#include "dialog.h"

Entrys::Entrys(QObject *parent)
    : QObject{parent}
{

}

void Entrys::setList(QString side) {
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
                    entry->setValue(Type::Directory, lecture->d_name, 0, modif, 0);
                }
            } else {
                if (S_ISDIR(buf.st_mode)) {
                    QString qstringTemp = lecture->d_name;
                    qstringTemp += "/";
                    entry->setValue(Type::Directory, qstringTemp, 0, modif, buf.st_mode);
                } else if (S_ISBLK(buf.st_mode)) {
                    entry->setValue(Type::DeviceBloc, lecture->d_name, size, modif, buf.st_mode); // TODO test if size
                } else if(S_ISCHR(buf.st_mode)) {
                    entry->setValue(Type::DeviceCharacter, lecture->d_name, size, modif, buf.st_mode); // TODO test if size
                } else if(S_ISFIFO(buf.st_mode)) {
                    entry->setValue(Type::Fifo, lecture->d_name, size, modif, buf.st_mode); // TODO test if size
                } else if(S_ISLNK(buf.st_mode)) {
                    entry->setValue(Type::SymbolicLink, lecture->d_name, size, modif, buf.st_mode); // TODO test if size
                } else if(S_ISSOCK(buf.st_mode)) {
                    entry->setValue(Type::Socket, lecture->d_name, size, modif, buf.st_mode); // TODO test if size
                } else if(S_ISREG(buf.st_mode)) {
                    entry->setValue(Type::File, lecture->d_name, size, modif, buf.st_mode);
                } else {
                    entry->setValue(Type::Other, lecture->d_name, size, modif, buf.st_mode);
                }
            }
            vec_entry.append(entry);
            row++;
        }
    }
    delete lecture;
    closedir(dir);
    std::sort(vec_entry.begin(), vec_entry.end(), Entrys::EntryCompare());
    this->hash_side_entry.insert(side, vec_entry);
}

void Entrys::setDir(QString dir, QString side) {
    this->hash_path.insert(side, dir);
    this->setList(side);
}

void Entrys::execCmd(QString cmd, QString side) {
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
                    if (swFirstTime) {
                        Mess::DispMess(e);
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
            break;
        }
        i++;
    }
}

QVector<Entry*> Entrys::getEntrys(QString side) {
    return this->hash_side_entry.take(side);
}

QString Entrys::minusOneLevel(QString dir) {
    QStringList list = dir.split('/');
    dir = "";
    for (int j = 0; j < (list.size() - 2); ++j) {
        if (j == 0) {
            dir = list[j];
        } else {
            dir += "/" + list[j];
        }
    }
    return dir + "/";
}

QString Entrys::getPath(QString side) {
    return this->hash_path[side];
}

Entry* Entrys::getEntry(QString side, QString key) {
    QVector<Entry*> entry = this->getEntrys(side);
    for (int i = 0; i < entry.length(); i++) {
        if (entry[i]->getName() == key) {
            return entry[i];
        }
    }
    return nullptr;
}

bool Entrys::EntryCompare::operator()(Entry *a, Entry *b) const {
    return(std::make_tuple(a->getOrder(), a->getName().toLower()) < std::make_tuple(b->getOrder(), b->getName().toLower()));
}
