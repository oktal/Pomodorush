#ifndef ACTIVITIESMODEL_H
#define ACTIVITIESMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include "sql/entities/activity.h"

class ActivityManager;

class ActivitiesModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Column { Urgent, Date, Type, Description, Estimation, Count };

    explicit ActivitiesModel(QObject *parent = 0);
    ~ActivitiesModel();

    Qt::ItemFlags flags(const QModelIndex &index) const;

    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role) const;

    bool addActivity(Activity &activity);
    bool setActivity(const QModelIndex &index, const Activity &activity);

    const Activity &activity(const QModelIndex &index) const;

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

private:
    QList<Activity> mActivities;
    QScopedPointer<ActivityManager> mManager;
};

#endif // ACTIVITIESMODEL_H
