#include "timerdialog.h"
#include "ui_timerdialog.h"
#include "interruptiondialog.h"

#include "settings.h"

#include <QSettings>
#include <QTimer>
#include <QDebug>
#include <QApplication>

#include <Phonon/MediaObject>
#include <Phonon/AudioOutput>
#include <QFile>
#include <QMessageBox>

static const QLatin1String &tickingSound = QLatin1String("sounds/clock_ticking_cut.wav");

TimerDialog::TimerDialog(const Todo &todo, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TimerDialog),
    mTodo(todo),
    mTimer(new QTimer(this)),
    mPeriod(Work),
    mBreakCount(0)
{
    ui->setupUi(this);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(onTimerTimeout()));
    ui->lblTask->setText(todo.description);

    Phonon::AudioOutput *audioOutput = new Phonon::AudioOutput(Phonon::NotificationCategory);
    mediaObject = new Phonon::MediaObject(this);
    Phonon::createPath(mediaObject, audioOutput);
    //mediaObject->setTransitionTime(-40);
    connect(mediaObject, SIGNAL(aboutToFinish()), this, SLOT(onSoundFinished()));

    QSettings settings(qApp->organizationName(), qApp->applicationName());
    const int pomodoroLength = settings.value(SETTING_POMODOROLENGTH).toInt();

    ui->lblTime->setText(QTime(0, pomodoroLength, 0).toString("mm:ss"));
}

TimerDialog::~TimerDialog()
{
    delete ui;
}

TimerDialog::Period TimerDialog::period() const
{
    return mPeriod;
}

void TimerDialog::disableTimer()
{
    mTimer->stop();
    mediaObject->stop();

    ui->btnInterruption->setEnabled(false);
    ui->btnNextPomodoro->setEnabled(false);
    ui->btnVoid->setEnabled(false);

    ui->lblPeriod->setText(tr("You have consumed all your pomodoro"));
}

void TimerDialog::startTimer()
{
    QSettings settings(qApp->organizationName(), qApp->applicationName());
    const int pomodoroLength = settings.value(SETTING_POMODOROLENGTH).toInt();
    mTimer->start(1000);
    mTime.setHMS(0, pomodoroLength, 0);
    mediaObject->setCurrentSource(Phonon::MediaSource(tickingSound));
    mediaObject->play();
}

void TimerDialog::onTimerTimeout()
{
    mTime = mTime.addSecs(-1);
    ui->lblTime->setText(mTime.toString("mm:ss"));
    QSettings settings(qApp->organizationName(), qApp->applicationName());
    if (mTime.second() == 0 && mTime.minute() == 0) {
        mTimer->stop();
        if (mPeriod == Work) {
            ++mBreakCount;
            mediaObject->setCurrentSource(Phonon::MediaSource("sounds/alarm_clock.wav"));
            mediaObject->play();
            mediaObject->clearQueue();
            mTimer->start(1000);

            const int pomodoroLongBreak = settings.value(SETTING_POMODOROSLONGBREAK).toInt();
            int breakTime = 0;
            if (mBreakCount % pomodoroLongBreak == 0) {
                breakTime = settings.value(SETTING_LONGBREAK).toInt();
            } else {
                breakTime = settings.value(SETTING_SHORTBREAK).toInt();
            }
            mTime.setHMS(0, breakTime, 0);
            ui->lblPeriod->setText(tr("Now take a rest !"));
            ui->btnInterruption->setDisabled(true);
            ui->btnVoid->setDisabled(true);
            ui->btnNextPomodoro->setDisabled(true);
            mPeriod = Rest;
            emit timerFinished(mTodo);
        }
        else {
            ui->btnNextPomodoro->setEnabled(true);
            ui->lblPeriod->setText(tr("Time to work !"));
            const int pomodoroLength = settings.value(SETTING_POMODOROLENGTH).toInt();
            ui->lblTime->setText(QTime(0, pomodoroLength, 0).toString("mm:ss"));
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

void TimerDialog::on_btnVoid_clicked()
{
    const int ret = QMessageBox::warning(this, tr("Warning"),
                                         tr("Do you really want to void this pomodoro ?"),
                                         QMessageBox::Yes, QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        mTimer->stop();
        ui->btnInterruption->setEnabled(false);
        ui->btnVoid->setEnabled(false);
        ui->btnNextPomodoro->setEnabled(true);
        ui->lblPeriod->setText(tr("Time to work !"));
        QSettings settings(qApp->organizationName(), qApp->applicationName());
        const int pomodoroLength = settings.value(SETTING_POMODOROLENGTH).toInt();
        ui->lblTime->setText(QTime(0, pomodoroLength, 0).toString("mm:ss"));
        mediaObject->stop();
        emit pomodoroCanceled(mTodo);
    }
}
