#ifndef TODOFILTERMODEL_H
#define TODOFILTERMODEL_H

#include <QSortFilterProxyModel>

class TodoFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit TodoFilterModel(QObject *parent = 0);
    
public slots:
    void setDisplayDone(bool display);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;

private:
    bool mDisplayDone;
    
};

#endif // TODOFILTERMODEL_H
