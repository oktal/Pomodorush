#include "tododelegate.h"
#include "sql/models/todomodel.h"

#include <QPainter>

TodoDelegate::TodoDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

void TodoDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == TodoModel::Interruption) {
        drawInterruptions(painter, option, index);
    }

    else if (index.column() == TodoModel::Pomodoro) {
        drawPomodoros(painter, option, index);
    }
    else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

void TodoDelegate::drawInterruptions(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);

    const QString &interruption = index.data().toString();
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight());
    }

    const int yOffset = option.rect.y() + 8;
    const int xOffset = option.rect.x() + 5;
    for (int i = 0; i < interruption.size(); ++i) {
        const QChar &c = interruption.at(i);
        if (c == QLatin1Char('x')) {
            painter->drawPixmap(xOffset, yOffset, QPixmap(":/cross-icon"));
            painter->translate(12.0, 0.0);
        }
        else if (c == QLatin1Char('-')) {
            painter->drawPixmap(xOffset, yOffset + 4, QPixmap(":/minus-icon"));
            painter->translate(13.0, 0.0);
        }
    }

    painter->restore();
}

void TodoDelegate::drawPomodoros(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_ASSERT(index.column() == TodoModel::Pomodoro);

    painter->save();

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);

    const QVariant &data = index.data();
    const Estimation &e = qvariant_cast<Estimation>(data);

    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight());
    }

    const int yOffset = option.rect.y() + 5;
    const int xOffset = option.rect.x() + 5;

    for (int i = 0; i < e.base; ++i) {
        painter->drawPixmap(xOffset, yOffset, QPixmap(":/square-blue-icon"));
        painter->translate(16.0, 0.0);
    }

    const QList<int> &reestimation = e.reestimation;
    if (reestimation.size() > 0) {
        for (int i = 0; i < reestimation[0]; ++i) {
            painter->drawPixmap(xOffset, yOffset, QPixmap(":/circle-blue-icon"));
            painter->translate(16.0, 0.0);
        }

        if (reestimation.size() > 1) {
            for (int i = 0; i < reestimation[1]; ++i) {
                painter->drawPixmap(xOffset, yOffset, QPixmap(":/circle-blue-icon").scaled(16, 16, Qt::KeepAspectRatio));
                painter->translate(16.0, 0.0);
            }
        }
    }

    painter->restore();

}
