#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QTableView>
#include "entry.h"
#include "entrys.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

struct ErrDirNotFound: public std::exception {
    QString description;
    const char *what() const throw() {
        return "Directory not found";
    }
};

enum Cmd {
    cd, None
};

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
    void setListUi(QString side);
    void execCmd(QString cmd, QString side);
    QString minusOneLevel(QString dir);
    void cellClick(QString side, QString dir, QString key);
};
#endif // DIALOG_H
