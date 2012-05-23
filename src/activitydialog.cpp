#include "activitydialog.h"
#include "ui_activitydialog.h"

#include "estimationdelegate.h"

#include <QDebug>
#include <QListView>

const int MaxPomodoro = 7;

ActivityDialog::ActivityDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ActivityDialog)
{
    ui->setupUi(this);

    QListView *lv = qobject_cast<QListView*>(ui->cmbEstimation->view());
    if (lv != 0) {
        lv->setResizeMode(QListView::Adjust);
    }

    ui->lblWarning->hide();
    ui->cmbDate->setDate(QDate::currentDate());
    ui->cmbEstimation->setItemDelegate(new EstimationDelegate(0, this));

    mActivity.id = -1;

    for (int i = 1; i <= MaxPomodoro; ++i) {
        ui->cmbEstimation->addItem(QString::number(i));
    }

    mActivity.estimation = 1;
}

ActivityDialog::ActivityDialog(const Activity &a, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ActivityDialog),
    mActivity(a)

{
    ui->setupUi(this);

    for (int i = 1; i <= MaxPomodoro; ++i) {
        ui->cmbEstimation->addItem(QString::number(i));
    }
    ui->cmbEstimation->setItemDelegate(new EstimationDelegate(0, this));

    ui->cmbDate->setDate(a.date);
    ui->txtType->setText(a.type);
    ui->txtDescription->setPlainText(a.description);
    ui->cmbEstimation->setCurrentIndex(ui->cmbEstimation->findText(QString::number(a.estimation)));
    ui->chkUrgent->setChecked(a.urgent);

    ui->lblWarning->hide();
}

ActivityDialog::~ActivityDialog()
{
    delete ui;
}

Activity ActivityDialog::activity() const
{
    return mActivity;
}

void ActivityDialog::on_cmbDate_dateChanged(const QDate &date)
{
    mActivity.date = date;
}

void ActivityDialog::on_txtType_textEdited(const QString &arg1)
{
    mActivity.type = arg1;
}

void ActivityDialog::on_txtDescription_textChanged()
{
    mActivity.description = ui->txtDescription->toPlainText();
}

void ActivityDialog::on_cmbEstimation_activated(int index)
{
    Q_UNUSED(index);
    mActivity.estimation = ui->cmbEstimation->currentText().toInt();
}

void ActivityDialog::on_chkUrgent_clicked(bool checked)
{
    mActivity.urgent = checked;
}
