#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "sql/SqlHelper.h"
#include "sql/models/activitiesmodel.h"
#include "sql/models/activitiesfiltermodel.h"
#include "widgets/pHeaderView.h"
#include "activitydialog.h"
#include "estimationdelegate.h"
#include "timerdialog.h"

#include "sql/models/todomodel.h"
#include "sql/models/todofiltermodel.h"
#include "sql/entities/todo.h"
#include "tododelegate.h"
#include "tododialog.h"
#include "reestimationdialog.h"
#include "settings.h"

#include <QActionGroup>
#include <QSignalMapper>
#include <QSqlTableModel>
#include <QDebug>
#include <QDate>

#include <QMessageBox>
#include <QApplication>
#include <QSettings>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mActivitiesModel(new ActivitiesModel(this)),
    mActivitiesFilterModel(new ActivitiesFilterModel(this)),
    mTodoModel(new TodoModel(QDate::currentDate(), this)),
    mTodoFilterModel(new TodoFilterModel(this)),
    mTimerDialog(0)
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
    pagesMapper->setMapping(ui->settingsAction, ui->settingsPage);

    connect(ui->homeAction, SIGNAL(triggered()), pagesMapper, SLOT(map()));
    connect(ui->activityListAction, SIGNAL(triggered()), pagesMapper, SLOT(map()));
    connect(ui->todoListAction, SIGNAL(triggered()), pagesMapper, SLOT(map()));
    connect(ui->settingsAction, SIGNAL(triggered()), pagesMapper, SLOT(map()));

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

    mTodoFilterModel->setSourceModel(mTodoModel);
    ui->tblTodo->setModel(mTodoFilterModel);
    ui->tblTodo->setItemDelegate(new TodoDelegate(this));

    pHeaderView * todoHeader = new pHeaderView(Qt::Horizontal, this);
    todoHeader->setProportionalSectionSizes(true);
    todoHeader->setSectionStretchFactor(TodoModel::Urgent, 5);
    todoHeader->setSectionStretchFactor(TodoModel::Type, 15);
    todoHeader->setSectionStretchFactor(TodoModel::Description, 40);
    todoHeader->setSectionStretchFactor(TodoModel::Interruption, 15);
    todoHeader->setSectionStretchFactor(TodoModel::Pomodoro, 25);
    ui->tblTodo->setHorizontalHeader(todoHeader);

    ui->lblCurrentDate->setText(QDate::currentDate().toString(Qt::SystemLocaleLongDate));
    ui->todoDateEdit->setDate(QDate::currentDate());
    readSettings();
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

void MainWindow::readSettings()
{
    QSettings settings(qApp->organizationName(), qApp->applicationName());
    ui->spinPomodoroLength->setValue(settings.value(SETTING_POMODOROLENGTH).toInt());
    ui->spinShortBreak->setValue(settings.value(SETTING_SHORTBREAK).toInt());
    ui->spinLongBreak->setValue(settings.value(SETTING_LONGBREAK).toInt());
    ui->spinPomodoroLongBreak->setValue(settings.value(SETTING_POMODOROSLONGBREAK).toInt());
    ui->chkTimerTicking->setChecked(settings.value(SETTING_TIMERTICKING).toBool());
    ui->chkTimerBeep->setChecked(settings.value(SETTING_TIMERBEEP).toBool());
}

void MainWindow::writeSettings()
{
    const int pomodoroLength = ui->spinPomodoroLength->value();
    const int shortBreak = ui->spinShortBreak->value();
    const int longBreak = ui->spinLongBreak->value();
    const int pomodoroLongBreak = ui->spinPomodoroLongBreak->value();
    const bool timerTicking = ui->chkTimerTicking->isChecked();
    const bool timerBeep = ui->chkTimerBeep->isChecked();

    QSettings settings(qApp->organizationName(), qApp->applicationName());
    settings.setValue(SETTING_POMODOROLENGTH, pomodoroLength);
    settings.setValue(SETTING_SHORTBREAK, shortBreak);
    settings.setValue(SETTING_LONGBREAK, longBreak);
    settings.setValue(SETTING_POMODOROSLONGBREAK, pomodoroLongBreak);
    settings.setValue(SETTING_TIMERTICKING, timerTicking);
    settings.setValue(SETTING_TIMERBEEP, timerBeep);
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
        todo.urgent = a.urgent;
        todo.done = false;
        todo.pomodoro_done = 0;
        todo.states.reserve(todo.estimation);
        for (int i = 0; i < todo.estimation; ++i) {
            todo.states.append(Todo::OnHold);
        }
        mTodoModel->addTodo(todo);
    }

}

void MainWindow::on_tblActivity_clicked(const QModelIndex &index)
{
    ui->btnEditActivity->setEnabled(index.isValid());
    ui->btnRemoveActivity->setEnabled(index.isValid());
    ui->btnPlanActivity->setEnabled(index.isValid());
}

void MainWindow::on_btnAddTodo_clicked()
{
    TodoDialog d;
    if (d.exec() == QDialog::Accepted) {
        Todo todo = d.todo();
        mTodoModel->addTodo(todo);
    }
}

void MainWindow::on_tblTodo_clicked(const QModelIndex &index)
{
    if (mTodoModel->date() == QDate::currentDate()) {
        const QModelIndex &mappedIndex = mTodoFilterModel->mapToSource(index);
        const bool done = mTodoModel->isDone(mappedIndex);
        const bool canStartPomodo = mTodoModel->remainingPomodoro(mappedIndex) > 0;
        ui->btnEditTodo->setEnabled(index.isValid());
        ui->btnRemoveTodo->setEnabled(index.isValid());
        ui->btnTaskDone->setEnabled(!done);
        ui->btnStartPomodoro->setEnabled(!done && canStartPomodo);
        ui->btnReestimate->setEnabled(!done && mTodoModel->canReestimate(mappedIndex));
    }
    else {
        ui->btnRemoveTodo->setEnabled(true);
    }
}

void MainWindow::on_btnEditTodo_clicked()
{
    const QModelIndex &index = ui->tblTodo->currentIndex();
    if (index.isValid()) {
        const QModelIndex &mappedIndex = mTodoFilterModel->mapToSource(index);
        const Todo &todo = mTodoModel->todo(mappedIndex);
        TodoDialog d(todo);
        if (d.exec() == QDialog::Accepted) {
            const Todo &todo = d.todo();
            mTodoModel->setTodo(mappedIndex, todo);
        }
    }
}

void MainWindow::on_btnRemoveTodo_clicked()
{
    const QModelIndex &index = ui->tblTodo->currentIndex();
    if (index.isValid()) {
        const QModelIndex &mappedIndex = mTodoFilterModel->mapToSource(index);
        const Todo &todo = mTodoModel->todo(mappedIndex);
        if (todo.pomodoro_done > 0) {
            const QMessageBox::StandardButton ret = QMessageBox::warning(this, tr("Warning"),
                                 tr("You are still working on this task. Do you really want to remove it ?"),
                                  QMessageBox::Yes | QMessageBox::No);
            if (ret == QMessageBox::No) {
                return;
            }
        }

        mTodoModel->removeRow(mappedIndex.row());
    }
}

void MainWindow::on_btnStartPomodoro_clicked()
{
    const QModelIndex &index = ui->tblTodo->currentIndex();
    if (index.isValid()) {
        const QModelIndex &mappedIndex = mTodoFilterModel->mapToSource(index);
        const Todo &todo = mTodoModel->todo(mappedIndex);

        delete mTimerDialog;
        mTimerDialog = new TimerDialog(todo, this);
        mTimerDialog->show();
        mTimerDialog->startTimer();
        connect(mTimerDialog, SIGNAL(timerFinished(Todo)), this, SLOT(onTimerFinished(Todo)));
        connect(mTimerDialog, SIGNAL(interrupted(Todo,Todo::Interruption)), this,
                SLOT(onInterrupted(Todo,Todo::Interruption)));
        connect(mTimerDialog, SIGNAL(pomodoroCanceled(Todo)), this, SLOT(onPomodoroCanceled(Todo)));
    }
}

/* TODO: refactor in one function */

void MainWindow::onTimerFinished(const Todo &todo)
{
    const QModelIndex &index = mTodoModel->todoIndex(todo.id);
    if (index.isValid()) {
        mTodoModel->finishPomodoro(index);
        const int remainingPomodoro = mTodoModel->remainingPomodoro(index);
        /* Postcon : remaining pomodoro must always be >= 0 */
        Q_ASSERT(remainingPomodoro >= 0);
        if (remainingPomodoro == 0) {
            mTimerDialog->disableTimer();
            ui->btnStartPomodoro->setEnabled(false);
        }
    }
}

void MainWindow::onPomodoroCanceled(const Todo &todo)
{

    const QModelIndex &index = mTodoModel->todoIndex(todo.id);
    if (index.isValid()) {
        mTodoModel->voidPomodoro(index);

        const int remainingPomodoro = mTodoModel->remainingPomodoro(index);
        /* Postcon : remaining pomodoro must always be >= 0 */
        Q_ASSERT(remainingPomodoro >= 0);
        if (remainingPomodoro == 0) {
            mTimerDialog->disableTimer();
            ui->btnStartPomodoro->setEnabled(false);
        }
    }
}

void MainWindow::onInterrupted(const Todo &todo, const Todo::Interruption &interruption)
{
    const QModelIndex &index = mTodoModel->todoIndex(todo.id);
    if (index.isValid()) {
        mTodoModel->addInterruption(index, interruption);
    }
}


void MainWindow::on_btnReestimate_clicked()
{
    const QModelIndex &index = ui->tblTodo->currentIndex();
    if (index.isValid()) {
        ReestimationDialog dialog;
        if (dialog.exec() == QDialog::Accepted) {
            const int reestimation = dialog.reestimation();
            const QModelIndex &mappedIndex = mTodoFilterModel->mapToSource(index);
            mTodoModel->reestimate(mappedIndex, reestimation);
            ui->btnReestimate->setEnabled(mTodoModel->canReestimate(mappedIndex));
            ui->btnStartPomodoro->setEnabled(true);
        }
    }
}

void MainWindow::on_btnTaskDone_clicked()
{
    const QModelIndex &index = ui->tblTodo->currentIndex();
    if (index.isValid()) {
        const QModelIndex &mappedIndex = mTodoFilterModel->mapToSource(index);
        mTodoModel->taskDone(mappedIndex);
        ui->btnTaskDone->setDisabled(true);
        ui->btnStartPomodoro->setDisabled(true);
        ui->btnReestimate->setDisabled(true);
    }
}

void MainWindow::on_btnHideDone_clicked(bool checked)
{
    mTodoFilterModel->setDisplayDone(!checked);
}

void MainWindow::on_btnSaveSettings_clicked()
{
    writeSettings();
    ui->btnSaveSettings->setEnabled(false);
}


void MainWindow::on_chkTimerTicking_clicked()
{
    ui->btnSaveSettings->setEnabled(true);
}

void MainWindow::on_chkTimerBeep_clicked()
{
    ui->btnSaveSettings->setEnabled(true);
}

void MainWindow::on_cmbLang_activated(int index)
{
    Q_UNUSED(index);
    ui->btnSaveSettings->setEnabled(true);
}

void MainWindow::on_spinPomodoroLength_editingFinished()
{
    ui->btnSaveSettings->setEnabled(true);
}

void MainWindow::on_spinShortBreak_editingFinished()
{
    ui->btnSaveSettings->setEnabled(true);
}

void MainWindow::on_spinLongBreak_editingFinished()
{
    ui->btnSaveSettings->setEnabled(true);
}

void MainWindow::on_spinPomodoroLongBreak_editingFinished()
{
    ui->btnSaveSettings->setEnabled(true);
}

void MainWindow::on_todoDateEdit_dateChanged(const QDate &date)
{
    mTodoModel->setDate(date);
    ui->lblCurrentDate->setText(date.toString(Qt::SystemLocaleLongDate));
    ui->btnEditTodo->setEnabled(false);
    ui->btnRemoveTodo->setEnabled(false);
    ui->btnStartPomodoro->setEnabled(false);
    ui->btnReestimate->setEnabled(false);
    ui->btnTaskDone->setEnabled(false);
}
