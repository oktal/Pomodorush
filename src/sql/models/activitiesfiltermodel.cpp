#include "activitiesfiltermodel.h"
#include "activitiesmodel.h"

ActivitiesFilterModel::ActivitiesFilterModel(QObject *parent) :
    QSortFilterProxyModel(parent),
    mDisplayUrgent(true)
{
}

void ActivitiesFilterModel::setDisplayUrgent(bool display)
{
    mDisplayUrgent = display;
    invalidateFilter();
}

bool ActivitiesFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (mDisplayUrgent) {
        return true;
    }

    const QModelIndex &urgentIndex = sourceModel()->index(source_row, ActivitiesModel::Urgent);
    const bool data = urgentIndex.data(Qt::EditRole).toBool();
    return data;
}
