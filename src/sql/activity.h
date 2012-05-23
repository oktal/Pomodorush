#ifndef ACTIVITY_H
#define ACTIVITY_H

#include <QString>
#include <QDate>

struct Activity
{
public:
    Activity();
    qint64 id;
    QDate date;
    QString type;
    QString description;
    int estimation;
    bool urgent;
};

#endif // ACTIVITY_H
