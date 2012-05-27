#include "interruptiondialog.h"
#include "ui_interruptiondialog.h"

InterruptionDialog::InterruptionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InterruptionDialog)
{
    ui->setupUi(this);
}

InterruptionDialog::~InterruptionDialog()
{
    delete ui;
}

Todo::Interruption InterruptionDialog::interruption() const
{
    Todo::Interruption interrupt;
    interrupt.type = ui->radInternal->isChecked() ?
                Todo::Interruption::Internal : Todo::Interruption::External;
    interrupt.reason = ui->txtReason->toPlainText();
    return interrupt;
}
