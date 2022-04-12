#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QTableView>
#include "entry.h"

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
    void on_lineEditCmdLeft_returnPressed();
    void on_lineEditCmdRight_returnPressed();
    void on_tableWidgetRight_cellEntered(int row, int column);
    void on_tableWidgetLeft_cellDoubleClicked(int row, int column);

private:
    Ui::Dialog *ui;
    QHash<QString, QString> hash_path;
    void setList(QString side);
    void setDir(QString dir, QString side);
    QString getPath (QString side);
    void execCmd(QString cmd, QString side);
    struct EntryCompare { bool operator()(Entry *a, Entry *b) const;};
};
#endif // DIALOG_H
