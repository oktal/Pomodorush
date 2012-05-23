#ifndef ACTIVITIESFILTERMODEL_H
#define ACTIVITIESFILTERMODEL_H

#include <QSortFilterProxyModel>

class ActivitiesFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit ActivitiesFilterModel(QObject *parent = 0);

public slots:
    void setDisplayUrgent(bool display);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
    
private:
    bool mDisplayUrgent;
    
};

#endif // ACTIVITIESFILTERMODEL_H
