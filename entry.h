#ifndef ENTRY_H
#define ENTRY_H

#include <QObject>
#include <QString>
#include <QDateTime>

enum Type {
    File, Directory, DeviceBloc, DeviceCharacter, Fifo, SymbolicLink, Socket, Other
};

class Entry : public QObject
{
    Q_OBJECT
public:
    explicit Entry(QObject *parent = nullptr);
    Entry(Type type, QString name, int size_file, QDateTime date_last_change, mode_t mode);
    void setValue(Type type, QString name, int size_file, QDateTime date_last_change, mode_t mode);
    QString getName();
    int getOrder();
    QString getSizeString(int decimals);
    QString getDateLastChangeString();
    QString getMode();
    Type getType();
signals:
private:
    QString name;
    Type type;
    int size_file;
    QDateTime date_last_change;
    mode_t mode;
};

#endif // ENTRY_H
