#ifndef INTERRUPTIONDIALOG_H
#define INTERRUPTIONDIALOG_H

#include <QDialog>
#include "sql/entities/todo.h"

namespace Ui {
class InterruptionDialog;
}

class InterruptionDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit InterruptionDialog(QWidget *parent = 0);
    ~InterruptionDialog();

    Todo::Interruption interruption() const;
    
private:
    Ui::InterruptionDialog *ui;
};

#endif // INTERRUPTIONDIALOG_H
