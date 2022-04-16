#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QTableView>
#include "entry.h"
#include "entrys.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();
protected:
    bool eventFilter(QObject* obj, QEvent* event);
private slots:
    void on_tableWidgetLeft_cellDoubleClicked(int row, int column);
    void on_tableWidgetRight_cellDoubleClicked(int row, int column);
private:
    Ui::Dialog *ui;
    Entrys *entrys;
    bool swDarkMode;
    void setListUi(QString side);
    void execCmd(QString cmd, QString side);
    void cellClick(QString side, QString dir, QString key);
};
#endif // DIALOG_H
