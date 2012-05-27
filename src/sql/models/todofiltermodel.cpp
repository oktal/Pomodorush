#include "todofiltermodel.h"
#include "todomodel.h"

TodoFilterModel::TodoFilterModel(QObject *parent) :
    QSortFilterProxyModel(parent),
    mDisplayDone(true)
{
}

void TodoFilterModel::setDisplayDone(bool display)
{
    mDisplayDone = display;
    invalidateFilter();
}

bool TodoFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_parent);
    if (mDisplayDone) {
        return true;
    }

    const QAbstractItemModel *model = sourceModel();
    const TodoModel *todoModel = qobject_cast<const TodoModel *>(model);
    Q_ASSERT(todoModel);

    const QModelIndex &index = todoModel->index(source_row, 0);
    return !todoModel->isDone(index);
}
