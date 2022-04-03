#include <QDateTime>
#include "entry.h"

Entry::Entry(QObject *parent)
    : QObject{parent}
{

}

Entry::Entry(Type type, QString name, int size_file, QDateTime date_last_modif)
{
    this->type = type;
    this->name = name;
    this->size_file = size_file;
    this->date_last_modif = date_last_modif;
}

void Entry::setValue(Type type, QString name, int size_file, QDateTime date_last_modif)
{
    this->type = type;
    this->name = name;
    this->size_file = size_file;
    this->date_last_modif = date_last_modif;
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

QString Entry::getSizeString() {
    if (this->type == Type::Directory) {
        return "DIR";
    } else if (this->type == Type::File) {
        return QString::number(this->size_file) + " bytes";
    } else {
        return "?"; // TODO complete this with a Switch
    }
}

QString Entry::getDateLastModifString() {
    return this->date_last_modif.toString("dd.MM.yyyy hh:mm:ss");
}
