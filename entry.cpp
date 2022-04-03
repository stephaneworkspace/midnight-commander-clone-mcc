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
