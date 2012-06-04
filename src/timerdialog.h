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

signals:
    void timerFinished(const Todo &todo);
    void interrupted(const Todo &todo, const Todo::Interruption &interuption);

public slots:
    void startTimer();

private slots:
    void onTimerTimeout();
    
    void on_btnInterruption_clicked();

    void on_btnNextPomodoro_clicked();

    void onSoundFinished();

private:
    Ui::TimerDialog *ui;
    Todo mTodo;
    QTimer *mTimer;
    QTime mTime;
    Period mPeriod;
    Phonon::MediaObject *mediaObject;
};

#endif // TIMERDIALOG_H
