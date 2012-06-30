#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>
#include "sql/entities/todo.h"

namespace Ui {
class MainWindow;
}

class ActivitiesModel;
class ActivitiesFilterModel;

class TodoModel;
class TodoFilterModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_btnAddActivity_clicked();

    void on_btnEditActivity_clicked();

    void on_btnRemoveActivity_clicked();

    void on_chkUrgentdActivities_clicked(bool checked);

    void on_tblActivity_doubleClicked(const QModelIndex &index);

    void on_btnPlanActivity_clicked();

    void on_tblActivity_clicked(const QModelIndex &index);

    void on_btnAddTodo_clicked();

    void on_tblTodo_clicked(const QModelIndex &index);

    void on_btnEditTodo_clicked();

    void on_btnRemoveTodo_clicked();

    void on_btnStartPomodoro_clicked();

    void onTimerFinished(const Todo &todo);
    void onInterrupted(const Todo &todo, const Todo::Interruption &interruption);
    void onPomodoroCanceled(const Todo &todo);

    void on_btnReestimate_clicked();

    void on_btnTaskDone_clicked();

    void on_btnHideDone_clicked(bool checked);

    void on_btnSaveSettings_clicked();

    void on_chkTimerTicking_clicked();

    void on_chkTimerBeep_clicked();

    void on_cmbLang_activated(int index);

    void on_spinPomodoroLength_editingFinished();

    void on_spinShortBreak_editingFinished();

    void on_spinLongBreak_editingFinished();

    void on_spinPomodoroLongBreak_editingFinished();

private:
    void editActivity(const QModelIndex &index);
    void readSettings();
    void writeSettings();

    Ui::MainWindow *ui;
    ActivitiesModel *mActivitiesModel;
    ActivitiesFilterModel *mActivitiesFilterModel;
    TodoModel *mTodoModel;
    TodoFilterModel *mTodoFilterModel;
};

#endif // MAINWINDOW_H
