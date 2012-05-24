#include "tododelegate.h"
#include "sql/models/todomodel.h"

TodoDelegate::TodoDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

void TodoDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
}
