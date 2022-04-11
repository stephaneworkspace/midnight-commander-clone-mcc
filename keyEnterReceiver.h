//
// Created by Stéphane on 11.04.22.
//

#ifndef MIDNIGHT_COMMANDER_CLONE_MCC_KEYENTERRECEIVER_H
#define MIDNIGHT_COMMANDER_CLONE_MCC_KEYENTERRECEIVER_H
#include <QObject>
#include <QKeyEvent>


class keyEnterReceiver : public QObject
{
    Q_OBJECT
protected:
    bool eventFilter(QObject* obj, QEvent* event);
};


#endif //MIDNIGHT_COMMANDER_CLONE_MCC_KEYENTERRECEIVER_H
