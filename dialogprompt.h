#ifndef DIALOGPROMPT_H
#define DIALOGPROMPT_H

#include <QDialog>
#include <sys/stat.h>
#include <dirent.h>

namespace Ui {
class DialogPrompt;
}

enum Prompt {
    F7, // mkdir
    F6 // rename move
};

class DialogPrompt : public QDialog
{
    Q_OBJECT

public:
    explicit DialogPrompt(QWidget *parent = nullptr, QString path = "", Prompt prompt = Prompt::F7, QString rename = "", QString origin = "");
    ~DialogPrompt();
    QString getPath();

private slots:
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();

private:
    Ui::DialogPrompt *ui;
    QWidget *parent;
    QString path;
    QString rename;
    QString origin;
    bool swDir; // TODO better with entry
    Prompt prompt;
};

#endif // DIALOGPROMPT_H
