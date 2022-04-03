#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QTableView>
#include "entry.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();

private:
    Ui::Dialog *ui;
    QVector<Entry*> vec_entry;
    void ls(QString repertoire);
    int countLs(QString repertoire);
    void entry(QString repertoire);
};
#endif // DIALOG_H
