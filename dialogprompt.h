#ifndef DIALOGPROMPT_H
#define DIALOGPROMPT_H

#include <QDialog>
#include <sys/stat.h>
#include <dirent.h>

namespace Ui {
class DialogPrompt;
}

class DialogPrompt : public QDialog
{
    Q_OBJECT

public:
    explicit DialogPrompt(QWidget *parent = nullptr, QString path = "");
    ~DialogPrompt();
    QString getPath(); // TODO protected

private slots:
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();

private:
    Ui::DialogPrompt *ui;
    QWidget *parent;
    QString path;
};

#endif // DIALOGPROMPT_H
