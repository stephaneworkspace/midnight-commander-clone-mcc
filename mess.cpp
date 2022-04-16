#include <QMessageBox>
#include "mess.h"

Mess::Mess(QObject *parent)
    : QObject{parent}
{

}

void Mess::DispMess(ErrDirNotFound &e) {
    QMessageBox msgBox;
    msgBox.setText(e.what());
    msgBox.setInformativeText(e.description);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.setStyleSheet("QLabel{min-width:500 px; font-size: 24px;} QPushButton{ width:250px; font-size: 18px; }");
    msgBox.exec();
}
