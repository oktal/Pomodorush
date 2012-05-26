#include "tododialog.h"
#include "ui_tododialog.h"

#include "estimationdelegate.h"

#include <QDate>
#include <QListView>

const int MaxPomodoro = 7;

TodoDialog::TodoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TodoDialog)
{
    ui->setupUi(this);
    ui->txtDate->setText(QDate::currentDate().toString(Qt::SystemLocaleLongDate));

    ui->lblWarning->hide();
    ui->lblWarnReestimate->hide();

    QListView *lv = qobject_cast<QListView*>(ui->cmbEstimation->view());
    if (lv != 0) {
        lv->setResizeMode(QListView::Adjust);
    }

    ui->cmbEstimation->setItemDelegate(new EstimationDelegate(0, this));


    for (int i = 1; i <= MaxPomodoro; ++i) {
        ui->cmbEstimation->addItem(QString::number(i));
    }
}

TodoDialog::TodoDialog(const Todo &todo, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TodoDialog),
    mTodo(todo)
{

    ui->setupUi(this);
    ui->txtDate->setText(QDate::currentDate().toString(Qt::SystemLocaleLongDate));

    for (int i = 1; i <= MaxPomodoro; ++i) {
        ui->cmbEstimation->addItem(QString::number(i));
    }
    ui->cmbEstimation->setItemDelegate(new EstimationDelegate(0, this));

    ui->txtType->setText(todo.type);
    ui->txtDescription->setPlainText(todo.description);
    ui->cmbEstimation->setCurrentIndex(ui->cmbEstimation->findText(QString::number(todo.estimation)));
    ui->chkUrgent->setChecked(todo.urgent);

    ui->lblWarning->hide();

    if (todo.pomodoro_done > 0) {
        ui->lblWarnReestimate->show();
        ui->cmbEstimation->setDisabled(true);
    }
    else {
        ui->lblWarnReestimate->hide();
    }
}

TodoDialog::~TodoDialog()
{
    delete ui;
}

Todo TodoDialog::todo() const
{
    return mTodo;
}

void TodoDialog::on_txtType_textEdited(const QString &arg1)
{
    mTodo.type = arg1;
}

void TodoDialog::on_txtDescription_textChanged()
{
    mTodo.description = ui->txtDescription->toPlainText();
}

void TodoDialog::on_cmbEstimation_activated(int index)
{
    Q_UNUSED(index);
    mTodo.estimation = ui->cmbEstimation->currentText().toInt();
}

void TodoDialog::on_chkUrgent_clicked(bool checked)
{
    mTodo.urgent = checked;
}
