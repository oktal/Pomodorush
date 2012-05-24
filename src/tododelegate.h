#ifndef TODODELEGATE_H
#define TODODELEGATE_H

#include <QStyledItemDelegate>

class TodoDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit TodoDelegate(QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    
};

#endif // TODODELEGATE_H
