#ifndef TODO_H
#define TODO_H

#include <QString>
#include <QDate>
#include <QList>
#include <QVariant>

struct Todo
{
    enum PomodoroState { Finished, Void, OnHold };

    struct Interruption {
        enum Type { Internal, External };

        Type type;
        QString reason;
    };

    Todo();
    qint64 id;
    QDate date;
    QString type;
    QString description;
    int estimation;
    QList<int> reestimation;
    bool done;
    bool urgent;
    int pomodoro_done;

    QList<Interruption> interruptions;
    QList<int> states;

};

Q_DECLARE_METATYPE(QList<int>)

#endif // TODO_H
