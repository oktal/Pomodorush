#ifndef TODOMODEL_H
#define TODOMODEL_H

#include <QAbstractTableModel>
#include <QDate>
#include <QScopedPointer>

#include "sql/entities/todo.h"

class TodoManager;

struct Estimation {
    int base;

    QList<int> reestimation;
};


class TodoModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TodoModel(const QDate &date = QDate::currentDate(), QObject *parent = 0);
    ~TodoModel();
    enum Column { Urgent, Type, Description, Interruption, Pomodoro, Count };

    Qt::ItemFlags flags(const QModelIndex &index) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role) const;

    bool addTodo(Todo &todo);
    bool setTodo(const QModelIndex &index, const Todo &todo);
    const Todo &todo(const QModelIndex &index) const;

    QModelIndex todoIndex(qint64 id) const;

    void finishPomodoro(const QModelIndex &index);
    void voidPomodoro(const QModelIndex &index);
    void addInterruption(const QModelIndex &index, const Todo::Interruption &interruption);
    bool canReestimate(const QModelIndex &index) const;
    void reestimate(const QModelIndex &index, int reestimation);
    void taskDone(const QModelIndex &index);
    bool isDone(const QModelIndex &index) const;
    int remainingPomodoro(const QModelIndex &index) const;

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

public slots:
    void setDate(const QDate &date);
    QDate date() const;

private:
    Estimation estimation(const QModelIndex &index) const;
    QString interruption(const QModelIndex &index) const;
    void changePomodoroState(const QModelIndex &index, Todo::PomodoroState state);

    QDate mDate;
    QScopedPointer<TodoManager> mManager;
    QList<Todo> mTodo;
    
};

Q_DECLARE_METATYPE(Estimation)

#endif // TODOMODEL_H
