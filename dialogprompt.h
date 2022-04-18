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

private:
    Ui::DialogPrompt *ui;
};

#endif // DIALOGPROMPT_H
