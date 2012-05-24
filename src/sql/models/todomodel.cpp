#include "todomodel.h"
#include "sql/todo.h"
#include "sql/SqlHelper.h"

#include <QDebug>
#include <QSqlError>

class TodoManager {
public:
    TodoManager() { }

    bool createTodo(Todo &todo) {
        SQL_QUERY(query);
        query.prepare("INSERT INTO todo(date, type, description, "
                      "estimation, done, pomodoro_done, urgent) "
                      "VALUES(:date, :type, :description, :estimation, "
                      ":done, :pomodoro_done, :urgent)");
        query.bindValue(":date", todo.date);
        query.bindValue(":type", todo.type);
        query.bindValue(":description", todo.description);
        query.bindValue(":estimation", todo.estimation);
        query.bindValue(":done", todo.done);
        query.bindValue(":pomodoro_done", todo.pomodoro_done);
        query.bindValue(":urgent", todo.urgent);

        const bool ok = SqlHelper::transaction();
        if (!query.exec()) {
            REPORT_SQL_ERROR(query);

            if (ok) {
                SqlHelper::rollback();
            }
            return false;
        }

        if (ok) {
            SqlHelper::commit();
        }

        return true;
    }

    QList<Todo> selectTodos(const QDate &date) {
        SQL_QUERY(query);
        query.prepare("SELECT id, date, type, description, estimation, done, "
                      "pomodoro_done, urgent FROM todo "
                      "WHERE date=:date");
        query.bindValue(":date", date);
        QList<Todo> todos;
        if (query.exec()) {
            while (query.next()) {
                Todo todo;
                todo.id = query.value(0).toInt();
                todo.date = query.value(1).toDate();
                todo.type = query.value(2).toString();
                todo.description = query.value(3).toString();
                todo.estimation = query.value(4).toInt();
                todo.done = query.value(5).toBool();
                todo.pomodoro_done = query.value(6).toInt();
                todo.urgent = query.value(7).toBool();
                todo.reestimation = selectReestimations(todo.id);
                todo.interruptions = selectInterruptions(todo.id);
                todos << todo;
            }
        }

        else {
            REPORT_SQL_ERROR(query);
        }

        return todos;
    }

    bool updateTodos(const Todo &todo) {
        SQL_QUERY(query);
        query.prepare("UPDATE TODO SET "
                      "date=:date, type=:type, "
                      "description=:description, "
                      "estimation=:estimation, "
                      "done=:done, "
                      "pomodoro_done=:pomodoroDone, "
                      "urgent=:urgent "
                      "WHERE id=:todoId");
        query.bindValue(":date", todo.date);
        query.bindValue(":type", todo.type);
        query.bindValue(":description", todo.description);
        query.bindValue(":estimation", todo.estimation);
        query.bindValue(":done", todo.done);
        query.bindValue(":pomodoroDone", todo.pomodoro_done);
        query.bindValue(":urgent", todo.urgent);
        query.bindValue(":todoId", todo.id);

        const bool ok = SqlHelper::transaction();
        if (!query.exec()) {
            REPORT_SQL_ERROR(query);
            if (ok) {
                SqlHelper::rollback();
            }
        }

        if (ok) {
            SqlHelper::commit();
        }

        return true;
    }

    bool deleteTodo(const Todo &todo) {
        const bool reestimation_delete = deleteReestimations(todo.id);

        if (!reestimation_delete) {
            return false;
        }

        SQL_QUERY(query);
        query.prepare("DELETE FROM todo WHERE id=:id");
        query.bindValue(":id", todo.id);

        const bool ok = SqlHelper::transaction();
        if (!query.exec()) {
            REPORT_SQL_ERROR(query);

            if (ok) {
                SqlHelper::rollback();
            }
        }

        if (ok) {
            SqlHelper::commit();
        }

        return true;
    }

private:
    QList<int> selectReestimations(int todoId) {
        SQL_QUERY(query);
        query.prepare("SELECT reestimation FROM todo_reestimation "
                      "WHERE todo_id=:todoId");
        query.bindValue(":todoId", todoId);
        QList<int> reestimations;
        if (query.exec()) {
            while (query.next()) {
                reestimations << query.value(0).toInt();
            }
        }
        else {
            REPORT_SQL_ERROR(query);
        }

        return reestimations;
    }

    bool deleteReestimations(int todoId) {
        SQL_QUERY(query);
        query.prepare("DELETE FROM todo_reestimation WHERE todo_id=:id");
        query.bindValue(":id", todoId);

        const bool ok = SqlHelper::transaction();
        if (!query.exec()) {
            REPORT_SQL_ERROR(query);

            if (ok) {
                SqlHelper::rollback();
            }
        }

        if (ok) {
            SqlHelper::commit();
        }

        return true;
    }

    QList<Todo::Interruption> selectInterruptions(int todoId) {
        SQL_QUERY(query);
        query.prepare("SELECT interruption_type, reason FROM interruption "
                      "WHERE todo_id=:todoId");
        query.bindValue(":todoId", todoId);

        QList<Todo::Interruption> interruptions;
        if (query.exec()) {
            while (query.next()) {
                Todo::Interruption interruption;
                interruption.type = static_cast<Todo::Interruption::Type>(
                            query.value(0).toInt());
                interruption.reason = query.value(1).toString();

                interruptions << interruption;
            }
        }
        else {
            REPORT_SQL_ERROR(query);
        }

        return interruptions;
    }

};

TodoModel::TodoModel(const QDate &date, QObject *parent) :
    QAbstractTableModel(parent),
    mDate(date),
    mManager(new TodoManager)
{
    mTodo = mManager->selectTodos(date);
}

TodoModel::~TodoModel()
{
}

Qt::ItemFlags TodoModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);
    if (index.column() == Urgent) {
        flags |= Qt::ItemIsUserCheckable;
    }

    return flags;
}

int TodoModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : Count;
}

int TodoModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : mTodo.count();
}

QVariant TodoModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case Urgent:
            return QLatin1String("U");
        case Type:
            return tr("Type");
        case Description:
            return tr("Description");
        case Interruption:
            return tr("Interruptions");
        case Pomodoro:
            return tr("Pomodoros");
        }
    }

    return QVariant();
}

QVariant TodoModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= mTodo.count()) {
        return QVariant();
    }

    const Todo &todo = mTodo.at(index.row());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case Urgent:
            return QVariant();
        case Type:
            return todo.type;
        case Description:
            return todo.description;
        case Interruption:
            return interruption(index);
            break;
        case Pomodoro:
            return QVariant::fromValue(estimation(index));
        }
    }

    else if (role == Qt::CheckStateRole && index.column() == Urgent) {
        return todo.urgent ? Qt::Checked : Qt::Unchecked;
    }

    return QVariant();
}

bool TodoModel::addTodo(Todo &todo)
{
    bool ok = false;
    if (mManager->createTodo(todo)) {
        beginInsertRows(QModelIndex(), mTodo.count(), mTodo.count());
        mTodo << todo;
        endInsertRows();
        ok = true;
    }

    return ok;
}

bool TodoModel::setTodo(const QModelIndex &index, const Todo &todo)
{
    bool ok = false;
    if (mManager->updateTodos(todo)) {
        const QModelIndex topLeft = this->index(index.row(), 0);
        const QModelIndex bottomRight = this->index(index.row(), Count - 1);
        emit dataChanged(topLeft, bottomRight);
        mTodo[index.row()] = todo;
        ok = true;
    }

    return ok;
}

const Todo &TodoModel::todo(const QModelIndex &index) const
{
    if (index.row() < 0 || index.row() >= mTodo.count()) {
        Q_ASSERT_X(false, Q_FUNC_INFO, "Bad Index");
    }

    return mTodo.at(index.row());
}

bool TodoModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || row + count >= mTodo.count()) {
        return false;
    }

    bool ok = true;
    int r = row;
    while (r < row + count && ok) {
        const Todo &t = mTodo.at(r++);
        ok = mManager->deleteTodo(t);
    }

    if (ok) {
        beginRemoveRows(parent, row, row + count - 1);
        for (int r = 0; r < count; ++r) {
            mTodo.removeAt(row);
        }
        endRemoveRows();
    }

    return ok;
}

void TodoModel::setDate(const QDate &date)
{
    beginResetModel();
    mDate = date;
    mTodo = mManager->selectTodos(date);
    endResetModel();
}

QDate TodoModel::date() const
{
    return mDate;
}

Estimation TodoModel::estimation(const QModelIndex &index) const
{
    if (index.row() < 0 || index.row() >= mTodo.count()) {
        Q_ASSERT_X(false, Q_FUNC_INFO, "Bad Index");
    }

    const Todo &todo = mTodo.at(index.row());

    Estimation e;
    e.base = todo.estimation;
    e.reestimation = todo.reestimation;

    return e;
}

QString TodoModel::interruption(const QModelIndex &index) const
{
    if (index.row() < 0 || index.row() >= mTodo.count()) {
        Q_ASSERT_X(false, Q_FUNC_INFO, "Bad Index");
    }

    const Todo &todo = mTodo.at(index.row());
    /* a 'x' symbolizes an internal interruption, where-as the '-' symbolizes
      an external interruption
    */
    QString ret;
    const QList<Todo::Interruption> &interruptions = todo.interruptions;
    foreach (const Todo::Interruption &i, interruptions) {
        if (i.type == Todo::Interruption::Internal) {
            ret += 'x';
        }
        else {
            ret += '-';
        }
    }

    return ret;
}
