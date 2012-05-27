#include "timerdialog.h"
#include "ui_timerdialog.h"
#include "interruptiondialog.h"

#include <QTimer>
#include <QDebug>

#include <QAudioOutput>
#include <QAudioFormat>
#include <QFile>

TimerDialog::TimerDialog(const Todo &todo, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TimerDialog),
    mTodo(todo),
    mTimer(new QTimer(this))
{
    ui->setupUi(this);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(onTimerTimeout()));
    ui->lblTask->setText(todo.description);
    alarmFile.setFileName("sounds/alarm_clock.wav");
}

TimerDialog::~TimerDialog()
{
    delete ui;
}

void TimerDialog::startTimer()
{
    mTimer->start(1000);
    mTime.setHMS(0, 0, 10);
}

void TimerDialog::onTimerTimeout()
{
    mTime = mTime.addSecs(-1);
    ui->lblTime->setText(mTime.toString("mm:ss"));
    if (mTime.second() == 0 && mTime.minute() == 0) {
        mTimer->stop();
        emit timerFinished(mTodo);
    }
}

void TimerDialog::on_btnInterruption_clicked()
{
    InterruptionDialog dialog;
    if (dialog.exec() == QDialog::Accepted) {
        const Todo::Interruption &interruption = dialog.interruption();
        emit interrupted(mTodo, interruption);
    }
}
