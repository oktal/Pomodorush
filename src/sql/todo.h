#ifndef TODO_H
#define TODO_H

#include <QString>
#include <QDate>
#include <QList>

struct Todo
{
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
};

#endif // TODO_H
