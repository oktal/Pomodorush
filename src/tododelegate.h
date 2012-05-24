#ifndef TODODELEGATE_H
#define TODODELEGATE_H

#include <QStyledItemDelegate>

class TodoDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit TodoDelegate(QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    void drawInterruptions(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void drawPomodoros(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    
};

#endif // TODODELEGATE_H
