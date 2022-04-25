#ifndef MESS_H
#define MESS_H

#include <QObject>
#include <filesystem>
#include "entrys.h"

namespace fs = std::filesystem;

class Mess : public QObject
{
    Q_OBJECT
public:
    explicit Mess(QObject *parent = nullptr);
    static void DispMess(ErrDirNotFound &e);
    static void DispMess(ErrKeyNotFound &e);
    static void DispMess(fs::filesystem_error &e);
    static void DispMess(std::bad_alloc &e);
    static void DispMess(std::exception &e);
    static void DispMessQString(QString msg, QString informativeMsg);
signals:

};

#endif // MESS_H
