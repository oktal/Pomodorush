#include "activitiesmodel.h"

#include "sql/SqlHelper.h"

#include <QDebug>
#include <QSqlError>

class ActivityManager {
public:
    ActivityManager() {

    }

    QList<Activity> selectActivities() {
        SQL_QUERY(query);
        query.prepare("SELECT id, date, type, description, estimation, urgent "
                      "FROM activity");
        QList<Activity> activities;
        if (query.exec()) {
            while (query.next()) {
                Activity a;
                a.id = query.value(0).toInt();
                a.date = query.value(1).toDate();
                a.type = query.value(2).toString();
                a.description = query.value(3).toString();
                a.estimation = query.value(4).toInt();
                a.urgent = query.value(5).toBool();
                activities << a;
            }
        }

        return activities;
    }

    bool createActivity(Activity &a) {
        SQL_QUERY(query);
        query.prepare("INSERT INTO activity(date, type, description, estimation, urgent) "
                      "VALUES(:date, :type, :description, :estimation, :urgent)");
        query.bindValue(":date", a.date);
        query.bindValue(":type", a.type);
        query.bindValue(":description", a.description);
        query.bindValue(":estimation", a.estimation);
        query.bindValue(":urgent", a.urgent ? 1 : 0);

        const bool ok = SqlHelper::transaction();
        if (!query.exec()) {
            REPORT_SQL_ERROR(query);
            if (ok) {
                SqlHelper::rollback();
            }
            return false;
        }

        if (ok) {
            SqlHelper::commit();
        }
        a.id = query.lastInsertId().toInt();
        return true;
    }

    bool updateActivity(const Activity &a) {
        SQL_QUERY(query);
        query.prepare("UPDATE activity SET "
                      "date=:date, type=:type, "
                      "description=:description, "
                      "estimation=:estimation, "
                      "urgent=:urgent "
                      "WHERE id=:id");
        query.bindValue(":date", a.date);
        query.bindValue(":type", a.type);
        query.bindValue(":description", a.description);
        query.bindValue(":estimation", a.estimation);
        query.bindValue(":urgent", a.urgent);
        query.bindValue(":id", a.id);

        const bool ok = SqlHelper::transaction();
        if (!query.exec()) {
            REPORT_SQL_ERROR(query);
            if (ok) {
                SqlHelper::rollback();
            }
            return false;
        }

        if (ok) {
            SqlHelper::commit();
        }
        return true;
    }

    bool deleteActivity(const Activity &a) {
        SQL_QUERY(query);
        query.prepare("DELETE FROM activity WHERE id=:id");
        query.bindValue(":id", a.id);

        const bool ok = SqlHelper::transaction();
        if (!query.exec()) {
            if (ok) {
                SqlHelper::rollback();
            }

            return false;
        }

        if (ok) {
            SqlHelper::commit();
        }
        return true;
    }

};

ActivitiesModel::ActivitiesModel(QObject *parent) :
    QAbstractTableModel(parent),
    mManager(new ActivityManager)
{
    mActivities = mManager->selectActivities();
}

ActivitiesModel::~ActivitiesModel()
{
}

Qt::ItemFlags ActivitiesModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);
    if (index.column() == Urgent) {
        flags |= Qt::ItemIsUserCheckable;
    }

    return flags;
}

int ActivitiesModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : Count;
}

int ActivitiesModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : mActivities.count();
}

QVariant ActivitiesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case Urgent:
            return "U";
        case Date:
            return tr("Date");
        case Type:
            return tr("Type");
        case Description:
            return tr("Description");
        case Estimation:
            return tr("Estimation");
        }
    }

    return QVariant();
}

QVariant ActivitiesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= mActivities.count()) {
        return QVariant();
    }

    const Activity &a = mActivities.at(index.row());
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
        case Date:
            return a.date;
        case Type:
            return a.type;
        case Description:
            return a.description;
        case Estimation:
            return a.estimation;
        }

        if (role == Qt::EditRole && index.column() == Urgent) {
            return a.urgent;
        }
    }

    else if (role == Qt::CheckStateRole) {
        if (index.column() == Urgent) {
            return a.urgent ? Qt::Checked : Qt::Unchecked;
        }
    }

    return QVariant();
}

bool ActivitiesModel::addActivity(Activity &activity)
{
    bool ok = false;
    if (mManager->createActivity(activity)) {
        beginInsertRows(QModelIndex(), mActivities.count(), mActivities.count());
        mActivities << activity;
        endInsertRows();
        ok = true;
    }

    return ok;
}

bool ActivitiesModel::setActivity(const QModelIndex &index, const Activity &activity)
{
    bool ok = false;
    if (mManager->updateActivity(activity)) {
        const QModelIndex topLeft = this->index(index.row(), 0);
        const QModelIndex bottomRight = this->index(index.row(), Count - 1);
        emit dataChanged(topLeft, bottomRight);
        mActivities[index.row()] = activity;
        ok = true;
    }

    return ok;
}

const Activity &ActivitiesModel::activity(const QModelIndex &index) const
{
    if (index.row() < 0 || index.row() >= mActivities.count()) {
        Q_ASSERT_X(false, Q_FUNC_INFO, "Bad Index");
    }

    return mActivities.at(index.row());
}

bool ActivitiesModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || row + count > mActivities.count()) {
        return false;
    }

    bool ok = true;
    int r = row;
    while (r < row + count && ok) {
        const Activity &a = mActivities.at(r++);
        ok = mManager->deleteActivity(a);
    }

    if (ok) {
        beginRemoveRows(parent, row, row + count - 1);
        for (int r = 0; r < count; ++r) {
            mActivities.removeAt(row);
        }
        endRemoveRows();
    }

    return ok;
}
