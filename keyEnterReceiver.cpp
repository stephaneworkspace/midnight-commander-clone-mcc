//
// Created by Stéphane on 11.04.22.
//

#include <QMessageBox>
#include "keyEnterReceiver.h"

bool keyEnterReceiver::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type()==QEvent::KeyPress) {
        QKeyEvent* key = static_cast<QKeyEvent*>(event);
        if ( (key->key()==Qt::Key_Enter) || (key->key()==Qt::Key_Return) ) {
            //Enter or return was pressed
            QMessageBox msgBox;
            msgBox.setText("Key enter");
            msgBox.setInformativeText("Cell entered");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.setStyleSheet("QLabel{min-width:500 px; font-size: 24px;} QPushButton{ width:250px; font-size: 18px; }");
            msgBox.exec();
        } else {
            return QObject::eventFilter(obj, event);
        }
        return true;
    } else {
        return QObject::eventFilter(obj, event);
    }
    return false;
}