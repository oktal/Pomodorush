#include "estimationdelegate.h"

#include <QPainter>
#include <QDebug>

EstimationDelegate::EstimationDelegate(int yOffset, QObject *parent) :
    QStyledItemDelegate(parent),
    mYOffset(yOffset)
{
}

void EstimationDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const int count = index.data(Qt::DisplayRole).toInt();

    painter->save();

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);

    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight());
    }

    const int yOffset = option.rect.y() + mYOffset;
    const int xOffset = option.rect.x() + 5;

    for (int i = 0; i < count; ++i) {
        painter->drawPixmap(xOffset, yOffset, QPixmap(":/delegate-icon").scaled(16, 16, Qt::KeepAspectRatio));
        painter->translate(16.0, 0.0);
    }

    painter->restore();
}
