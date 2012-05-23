#ifndef ACTIVITYDIALOG_H
#define ACTIVITYDIALOG_H

#include <QDialog>

#include "sql/activity.h"

namespace Ui {
class ActivityDialog;
}

class ActivityDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ActivityDialog(QWidget *parent = 0);
    explicit ActivityDialog(const Activity &a, QWidget *parent = 0);
    ~ActivityDialog();

    Activity activity() const;
    
private slots:
    void on_cmbDate_dateChanged(const QDate &date);

    void on_txtType_textEdited(const QString &arg1);

    void on_txtDescription_textChanged();

    void on_cmbEstimation_activated(int index);

    void on_chkUrgent_clicked(bool checked);

private:
    Ui::ActivityDialog *ui;
    Activity mActivity;
};

#endif // ACTIVITYDIALOG_H
