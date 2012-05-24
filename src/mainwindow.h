#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>

namespace Ui {
class MainWindow;
}

class ActivitiesModel;
class ActivitiesFilterModel;

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

private:
    void editActivity(const QModelIndex &index);

    Ui::MainWindow *ui;
    ActivitiesModel *mActivitiesModel;
    ActivitiesFilterModel *mActivitiesFilterModel;
};

#endif // MAINWINDOW_H
