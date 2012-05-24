#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "sql/SqlHelper.h"
#include "sql/models/activitiesmodel.h"
#include "sql/models/activitiesfiltermodel.h"
#include "widgets/pHeaderView.h"
#include "activitydialog.h"
#include "estimationdelegate.h"

#include "sql/models/todomodel.h"
#include "sql/todo.h"
#include "tododelegate.h"

#include <QActionGroup>
#include <QSignalMapper>
#include <QSqlTableModel>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mActivitiesModel(new ActivitiesModel(this)),
    mActivitiesFilterModel(new ActivitiesFilterModel(this)),
    mTodoModel(new TodoModel(QDate::currentDate(), this))
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

    ui->tblTodo->setModel(mTodoModel);
    ui->tblTodo->setItemDelegate(new TodoDelegate(this));

    pHeaderView * todoHeader = new pHeaderView(Qt::Horizontal, this);
    todoHeader->setProportionalSectionSizes(true);
    todoHeader->setSectionStretchFactor(TodoModel::Urgent, 5);
    todoHeader->setSectionStretchFactor(TodoModel::Type, 15);
    todoHeader->setSectionStretchFactor(TodoModel::Description, 40);
    todoHeader->setSectionStretchFactor(TodoModel::Interruption, 15);
    todoHeader->setSectionStretchFactor(TodoModel::Pomodoro, 25);
    ui->tblTodo->setHorizontalHeader(todoHeader);

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
        editActivity(mappedIndex);
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

void MainWindow::on_tblActivity_doubleClicked(const QModelIndex &index)
{
    const QModelIndex mappedIndex = mActivitiesFilterModel->mapToSource(index);
    editActivity(mappedIndex);
}

void MainWindow::editActivity(const QModelIndex &index)
{
    const Activity &activity = mActivitiesModel->activity(index);
    ActivityDialog d(activity);
    if (d.exec() == QDialog::Accepted) {
        const Activity &a = d.activity();
        mActivitiesModel->setActivity(index, a);
    }
}

void MainWindow::on_btnPlanActivity_clicked()
{
    const QModelIndex &index = ui->tblActivity->currentIndex();
    if (index.isValid()) {
        const QModelIndex &mappedIndex = mActivitiesFilterModel->mapToSource(index);
        Q_ASSERT(mappedIndex.isValid());

        const Activity &a = mActivitiesModel->activity(mappedIndex);
        Todo todo;
        todo.date = QDate::currentDate();
        todo.type = a.type;
        todo.description = a.description;
        todo.estimation = a.estimation;
        todo.pomodoro_done = 0;
        todo.urgent = a.urgent;
        todo.done = false;
        mTodoModel->addTodo(todo);
    }

}
