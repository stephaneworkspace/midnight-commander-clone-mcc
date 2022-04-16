#ifndef MESS_H
#define MESS_H

#include <QObject>
#include "entrys.h"

class Mess : public QObject
{
    Q_OBJECT
public:
    explicit Mess(QObject *parent = nullptr);
    static void DispMess(ErrDirNotFound &e);
signals:

};

#endif // MESS_H
