#include <QDateTime>
#include "entry.h"

Entry::Entry(QObject *parent)
    : QObject{parent}
{

}

Entry::Entry(Type type, QString name, int size_file, QDateTime date_last_change, mode_t mode)
{
    this->type = type;
    this->name = name;
    this->size_file = size_file;
    this->date_last_change = date_last_change;
    this->mode = mode;
}

void Entry::setValue(Type type, QString name, int size_file, QDateTime date_last_change, mode_t mode)
{
    this->type = type;
    this->name = name;
    this->size_file = size_file;
    this->date_last_change = date_last_change;
    this->mode = mode;
}

QString Entry::getName()
{
    return this->name;
}

int Entry::getOrder() {
    switch (this->type) {
        case Type::File:
            return 2;
        case Type::Directory:
            return 0;
        default:
            return 1;
    }
}

QString Entry::getSizeString(int decimals) {
    if (this->type == Type::Directory) {
        return "DIR";
    } else if (this->type == Type::File) {
        int bytes = this->size_file;
        if (bytes == 0) return "0 Bytes";
        const int K = 1024;
        const QStringList SIZES = { "Bytes", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB" };
        const int I =  std::floor((std::log(bytes) / std::log(K)));
        int dm = decimals < 0 ? 0 : decimals;
        if (I == 0) dm = 0;
        return QString::number((bytes / std::pow(K, I)),'f', dm) + " " + SIZES[I];
    } else {
        return "?"; // TODO test and complete this with a Switch
    }
}

QString Entry::getDateLastChangeString() {
    return this->date_last_change.toString("dd.MM.yyyy hh:mm:ss");
}

QString Entry::getMode() {
    QString buf;
    const char chars[] = "rwxrwxrwx";
    for (size_t i = 0; i < 9; i++) {
        buf += (this->mode & (1 << (8-i))) ? chars[i] : '-';
    }
    return buf;
}