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
    mTimer(new QTimer(this)),
    mPeriod(Work)
{
    ui->setupUi(this);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(onTimerTimeout()));
    ui->lblTask->setText(todo.description);
}

TimerDialog::~TimerDialog()
{
    delete ui;
}

TimerDialog::Period TimerDialog::period() const
{
    return mPeriod;
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
        if (mPeriod == Work) {
            emit timerFinished(mTodo);
            mTimer->start(1000);
            mTime.setHMS(0, 1, 0);
            ui->lblPeriod->setText(tr("Now take a rest !"));
            ui->btnInterruption->setDisabled(true);
            ui->btnVoid->setDisabled(true);
            ui->btnNextPomodoro->setDisabled(true);
            mPeriod = Rest;
        }
        else {
            ui->btnNextPomodoro->setEnabled(true);
            ui->lblPeriod->setText(tr("Time to work !"));
            ui->lblTime->setText("25:00");
        }
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
