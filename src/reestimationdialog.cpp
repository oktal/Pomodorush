#include "reestimationdialog.h"
#include "ui_reestimationdialog.h"

#include <QButtonGroup>

ReestimationDialog::ReestimationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReestimationDialog)
{
    ui->setupUi(this);

    mButtonGroup = new QButtonGroup(this);
    mButtonGroup->addButton(ui->radOne, 1);
    mButtonGroup->addButton(ui->radTwo, 2);
    mButtonGroup->addButton(ui->radThree, 3);
}

ReestimationDialog::~ReestimationDialog()
{
    delete ui;
}

int ReestimationDialog::reestimation() const
{
    return mButtonGroup->checkedId();
}
