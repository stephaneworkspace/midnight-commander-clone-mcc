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
    QHash<QString, QString> dir;
    void setList(QString side);
    void setDir(QString dir, QString side);
    QString getDir(QString side);
    struct EntryCompare { bool operator()(Entry *a, Entry *b) const;};
};
#endif // DIALOG_H
