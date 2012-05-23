#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "sql/SqlHelper.h"
#include "sql/models/activitiesmodel.h"
#include "sql/models/activitiesfiltermodel.h"
#include "widgets/pHeaderView.h"
#include "activitydialog.h"
#include "estimationdelegate.h"

#include <QActionGroup>
#include <QSignalMapper>
#include <QSqlTableModel>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mActivitiesModel(new ActivitiesModel(this)),
    mActivitiesFilterModel(new ActivitiesFilterModel(this))
{
    ui->setupUi(this);

    QActionGroup *menuGroup = new QActionGroup(this);
    menuGroup->addAction(ui->homeAction);
    menuGroup->addAction(ui->activityListAction);
    menuGroup->addAction(ui->todoListAction);
    menuGroup->addAction(ui->reportListAction);
    menuGroup->addAction(ui->settingsAction);

    QSignalMapper *pagesMapper = new QSignalMapper(this);
    pagesMapper->setMapping(ui->homeAction, ui->homePage);
    pagesMapper->setMapping(ui->activityListAction, ui->activityPage);
    pagesMapper->setMapping(ui->todoListAction, ui->todoPage);

    connect(ui->homeAction, SIGNAL(triggered()), pagesMapper, SLOT(map()));
    connect(ui->activityListAction, SIGNAL(triggered()), pagesMapper, SLOT(map()));
    connect(ui->todoListAction, SIGNAL(triggered()), pagesMapper, SLOT(map()));

    connect(pagesMapper, SIGNAL(mapped(QWidget*)), ui->stackedWidget, SLOT(setCurrentWidget(QWidget*)));

    ui->homeAction->setChecked(true);
    ui->stackedWidget->setCurrentWidget(ui->homePage);

    mActivitiesFilterModel->setSourceModel(mActivitiesModel);

    ui->tblActivity->setModel(mActivitiesFilterModel);
    ui->tblActivity->setItemDelegateForColumn(ActivitiesModel::Estimation, new EstimationDelegate(5, this));

    pHeaderView *activityHeader = new pHeaderView(Qt::Horizontal, this);
    activityHeader->setProportionalSectionSizes(true);
    activityHeader->setSectionStretchFactor(ActivitiesModel::Urgent, 5);
    activityHeader->setSectionStretchFactor(ActivitiesModel::Date, 15);
    activityHeader->setSectionStretchFactor(ActivitiesModel::Type, 20);
    activityHeader->setSectionStretchFactor(ActivitiesModel::Description, 40);
    activityHeader->setSectionStretchFactor(ActivitiesModel::Estimation, 20);

    ui->tblActivity->setHorizontalHeader(activityHeader);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnAddActivity_clicked()
{
    ActivityDialog d;
    if (d.exec() == QDialog::Accepted) {
        Activity a = d.activity();
        mActivitiesModel->addActivity(a);
    }
}

void MainWindow::on_btnEditActivity_clicked()
{
    const QModelIndex &index = ui->tblActivity->currentIndex();
    if (index.isValid()) {
        const QModelIndex &mappedIndex = mActivitiesFilterModel->mapToSource(index);
        const Activity &activity = mActivitiesModel->activity(mappedIndex);
        ActivityDialog d(activity);
        if (d.exec() == QDialog::Accepted) {
            const Activity &a = d.activity();
            mActivitiesModel->setActivity(mappedIndex, a);
        }
    }
}

void MainWindow::on_btnRemoveActivity_clicked()
{
    const QModelIndex &index = ui->tblActivity->currentIndex();
    if (index.isValid()) {
        const QModelIndex &mappedIndex = mActivitiesFilterModel->mapToSource(index);
        mActivitiesModel->removeRow(mappedIndex.row());
    }

}

void MainWindow::on_chkUrgentdActivities_clicked(bool checked)
{
    mActivitiesFilterModel->setDisplayUrgent(!checked);
}
