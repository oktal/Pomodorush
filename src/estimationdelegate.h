#ifndef ESTIMATIONDELEGATE_H
#define ESTIMATIONDELEGATE_H

#include <QStyledItemDelegate>

class EstimationDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit EstimationDelegate(int yOffset, QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    int mYOffset;
    
};

#endif // ESTIMATIONDELEGATE_H
