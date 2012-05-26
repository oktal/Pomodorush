#ifndef TODO_H
#define TODO_H

#include <QString>
#include <QDate>
#include <QList>

struct Todo
{
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
    int pomodoro_done;
    bool done;
    bool urgent;

    QList<Interruption> interruptions;

};

#endif // TODO_H
