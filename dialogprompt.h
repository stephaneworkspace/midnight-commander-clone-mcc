#ifndef DIALOGPROMPT_H
#define DIALOGPROMPT_H

#include <QDialog>

namespace Ui {
class DialogPrompt;
}

class DialogPrompt : public QDialog
{
    Q_OBJECT

public:
    explicit DialogPrompt(QWidget *parent = nullptr);
    ~DialogPrompt();

private slots:
    void on_buttonBox_rejected();

    void on_buttonBox_accepted();

private:
    Ui::DialogPrompt *ui;
    QWidget *parent;
};

#endif // DIALOGPROMPT_H
