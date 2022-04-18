#ifndef DIALOGPROMPT_H
#define DIALOGPROMPT_H

#include <QDialog>
#include <sys/stat.h>
#include <dirent.h>

namespace Ui {
class DialogPrompt;
}

enum Prompt {
    F7,
};

class DialogPrompt : public QDialog
{
    Q_OBJECT

public:
    explicit DialogPrompt(QWidget *parent = nullptr, QString path = "", Prompt prompt = Prompt::F7);
    ~DialogPrompt();
    QString getPath();

private slots:
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();

private:
    Ui::DialogPrompt *ui;
    QWidget *parent;
    QString path;
    Prompt prompt;
};

#endif // DIALOGPROMPT_H
