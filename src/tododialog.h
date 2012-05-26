#ifndef TODODIALOG_H
#define TODODIALOG_H

#include <QDialog>

#include "sql/todo.h"

namespace Ui {
class TodoDialog;
}

class TodoDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit TodoDialog(QWidget *parent = 0);
    explicit TodoDialog(const Todo &todo, QWidget *parent = 0);
    ~TodoDialog();

    Todo todo() const;
    
private slots:
    void on_txtType_textEdited(const QString &arg1);

    void on_txtDescription_textChanged();

    void on_cmbEstimation_activated(int index);

    void on_chkUrgent_clicked(bool checked);

private:
    Ui::TodoDialog *ui;
    Todo mTodo;
};

#endif // TODODIALOG_H
