#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QTableView>
#include <filesystem>
#include "entry.h"
#include "entrys.h"
#include "dialogprompt.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

namespace fs = std::filesystem;

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
    void on_pushButton_F7_clicked();
    void on_tableWidgetLeft_cellClicked(int row, int column);

    void on_tableWidgetRight_cellClicked(int row, int column);

    void on_tableWidgetLeft_cellEntered(int row, int column);

    void on_tableWidgetRight_cellEntered(int row, int column);

    void on_pushButton_F6_clicked();

    void on_pushButton_F5_clicked();

private:
    Ui::Dialog *ui;
    DialogPrompt *dialogPrompt;
    Entrys *entrys;
    QString sideFocus;
    bool swDarkMode;
    void setListUi(QString side);
    void execCmd(QString cmd, QString side);
    void cellClick(QString side, QString dir, QString key);
};
#endif // DIALOG_H
