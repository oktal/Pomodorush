#ifndef TIMERDIALOG_H
#define TIMERDIALOG_H

#include <QDialog>
#include <QTime>
#include <QFile>
#include <Phonon/MediaObject>
#include "sql/entities/todo.h"

namespace Ui {
class TimerDialog;
}

class QTimer;

class TimerDialog : public QDialog
{
    Q_OBJECT
    
public:
    enum Period { Work, Rest };
    explicit TimerDialog(const Todo &todo, QWidget *parent = 0);
    ~TimerDialog();
    Period period() const;

    void disableTimer();

signals:
    void timerFinished(const Todo &todo);
    void interrupted(const Todo &todo, const Todo::Interruption &interuption);
    void pomodoroCanceled(const Todo &todo);

public slots:
    void startTimer();

private slots:
    void onTimerTimeout();
    
    void on_btnInterruption_clicked();

    void on_btnNextPomodoro_clicked();

    void onSoundFinished();

    void on_btnVoid_clicked();

private:
    Ui::TimerDialog *ui;
    Todo mTodo;
    QTimer *mTimer;
    QTime mTime;
    Period mPeriod;
    Phonon::MediaObject *mediaObject;
    int mBreakCount;
};

#endif // TIMERDIALOG_H
