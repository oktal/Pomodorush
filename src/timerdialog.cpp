#include "timerdialog.h"
#include "ui_timerdialog.h"
#include "interruptiondialog.h"

#include <QTimer>
#include <QDebug>

#include <Phonon/MediaObject>
#include <Phonon/AudioOutput>
#include <QFile>

static const QLatin1String &tickingSound = QLatin1String("sounds/clock_ticking_cut.wav");

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

    Phonon::AudioOutput *audioOutput = new Phonon::AudioOutput(Phonon::NotificationCategory);
    mediaObject = new Phonon::MediaObject(this);
    Phonon::createPath(mediaObject, audioOutput);
    mediaObject->setTransitionTime(-40);
    connect(mediaObject, SIGNAL(aboutToFinish()), this, SLOT(onSoundFinished()));
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
    mediaObject->setCurrentSource(Phonon::MediaSource(tickingSound));
    mediaObject->play();
}

void TimerDialog::onTimerTimeout()
{
    mTime = mTime.addSecs(-1);
    ui->lblTime->setText(mTime.toString("mm:ss"));
    if (mTime.second() == 0 && mTime.minute() == 0) {
        mTimer->stop();
        if (mPeriod == Work) {
            emit timerFinished(mTodo);
            mediaObject->setCurrentSource(Phonon::MediaSource("sounds/alarm_clock.wav"));
            mediaObject->play();
            mediaObject->clearQueue();
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

void TimerDialog::on_btnNextPomodoro_clicked()
{
    startTimer();
    ui->btnInterruption->setEnabled(true);
    ui->btnVoid->setEnabled(true);
    ui->btnNextPomodoro->setDisabled(true);
    mPeriod = Work;
}

void TimerDialog::onSoundFinished()
{
    if (mPeriod == Work) {
        mediaObject->enqueue(Phonon::MediaSource(tickingSound));
    }
}
