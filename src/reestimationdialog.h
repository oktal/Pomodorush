#ifndef REESTIMATIONDIALOG_H
#define REESTIMATIONDIALOG_H

#include <QDialog>

namespace Ui {
class ReestimationDialog;
}

class QButtonGroup;

class ReestimationDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ReestimationDialog(QWidget *parent = 0);
    ~ReestimationDialog();

    int reestimation() const;
    
private:
    Ui::ReestimationDialog *ui;
    QButtonGroup *mButtonGroup;
};

#endif // REESTIMATIONDIALOG_H
