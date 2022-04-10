#include "dialog.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog w;
    w.setWindowTitle("Midnight Commander Clone - By Stéphane Bressani");
    w.show();
    return a.exec();
}
