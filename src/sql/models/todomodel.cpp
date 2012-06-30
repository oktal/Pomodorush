#include "todomodel.h"
#include "sql/SqlHelper.h"

#include <QDebug>
#include <QSqlError>
#include <QFont>
#include <QApplication>
#include <QColor>

#include <numeric>

static const int MaxReestimation = 2;

enum Test { Zib, Zab };

class TodoManager {
public:
    TodoManager() { }

    bool createTodo(Todo &todo) {
        SQL_QUERY(query);
        query.prepare("INSERT INTO todo(date, type, description, "
                      "estimation, done, pomodoro_done, urgent) "
                      "VALUES(:date, :type, :description, :estimation, "
                      ":pomodoro_done, :done, :urgent)");
        query.bindValue(":date", todo.date);
        query.bindValue(":type", todo.type);
        query.bindValue(":description", todo.description);
        query.bindValue(":estimation", todo.estimation);
        query.bindValue(":done", todo.done);
        query.bindValue(":urgent", todo.urgent);
        query.bindValue(":pomodoro_done", todo.pomodoro_done);

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

                todo.states = selectStates(todo.id, todo.estimation, todo.reestimation);
                todos << todo;
            }
        }

        else {
            REPORT_SQL_ERROR(query);
        }

        return todos;
    }

    bool updateTodo(const Todo &todo) {
        SQL_QUERY(query);
        query.prepare("UPDATE TODO SET "
                      "date=:date, type=:type, "
                      "description=:description, "
                      "estimation=:estimation, "
                      "done=:done, "
                      "pomodoro_done=:pomodoro_done, "
                      "urgent=:urgent "
                      "WHERE id=:todoId");
        query.bindValue(":date", todo.date);
        query.bindValue(":type", todo.type);
        query.bindValue(":description", todo.description);
        query.bindValue(":estimation", todo.estimation);
        query.bindValue(":done", todo.done);
        query.bindValue(":pomodoro_done", todo.pomodoro_done);
        query.bindValue(":urgent", todo.urgent);
        query.bindValue(":todoId", todo.id);

        const bool ok = SqlHelper::transaction();
        if (!query.exec()) {
            REPORT_SQL_ERROR(query);
            if (ok) {
                SqlHelper::rollback();
            }
        }

        const bool updateStates = updateTodoPomodoros(todo);
        if (!updateStates) {
            if (ok) {
                SqlHelper::rollback();
            }
            return false;
        }

        const bool reestimations = updateReestimations(todo);
        if (!reestimations) {
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

    QList<int> selectStates(int todoId, int estimation, const QList<int> &reestimation) {
//        int totalPomodoro = todo.estimation;
//        foreach (int reestimation, todo.reestimation) {
//            totalPomodoro += reestimation;
//        }
        const int totalPomodoro = std::accumulate(reestimation.begin(), reestimation.end(), estimation,
                                                  std::plus<int>());
        QList<int> states;
        states.reserve(totalPomodoro);
        for (int i = 0; i < totalPomodoro; ++i) {
            states.append(Todo::OnHold);
        }

        SQL_QUERY(query);
        query.prepare("SELECT state, number FROM pomodoro_state WHERE todo_id=:todoId");
        query.bindValue(":todoId", todoId);

        if (query.exec()) {
            int n = 0;
            while (query.next()) {
                Todo::PomodoroState state =
                        static_cast<Todo::PomodoroState>(query.value(0).toInt());
                states[n++] = state;
            }
        }
        else {
            REPORT_SQL_ERROR(query);
        }
        return states;
    }

    bool updateTodoPomodoros(const Todo &todo) {
        SQL_QUERY(query);
        query.prepare("DELETE FROM pomodoro_state WHERE todo_id=:todoId");
        query.bindValue(":todoId", todo.id);

        if (!query.exec()) {
            REPORT_SQL_ERROR(query);
            return false;
        }

        query.prepare("INSERT INTO pomodoro_state(todo_id, state, number) VALUES(:todoId, :state, :number)");
        query.bindValue(":todoId", todo.id);
        query.bindValue(":number", 0); /* Unused */
        foreach (int state, todo.states) {
            query.bindValue(":state", state);
            if (!query.exec()) {
                REPORT_SQL_ERROR(query);
                return false;
            }
        }

        return true;
    }

    bool updateReestimations(const Todo &todo) {
        SQL_QUERY(query);
        query.prepare("DELETE FROM todo_reestimation WHERE todo_id=:todoId");
        query.bindValue(":todoId", todo.id);

        if (!query.exec()) {
            REPORT_SQL_ERROR(query);
            return false;
        }

        query.prepare("INSERT INTO todo_reestimation(todo_id, reestimation) VALUES(:todoId, :reestimation)");
        query.bindValue(":todoId", todo.id);
        foreach (int reestimation, todo.reestimation) {
            query.bindValue(":reestimation", reestimation);
            if (!query.exec()) {
                REPORT_SQL_ERROR(query);
                return false;
            }
        }

        return true;
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
    else if (role == Qt::EditRole && index.column() == Pomodoro) {
        /* Cheat */
        QVariantList states;
        qCopy(todo.states.begin(), todo.states.end(), std::back_inserter(states));
        return states;
    }
    else if (role == Qt::FontRole && (index.column() == Type ||
             index.column() == Description)) {
        QFont font(qApp->font());
        if (todo.done) {
            font.setStrikeOut(true);
        }
        return font;
    }
    else if (role == Qt::BackgroundRole) {
        if (todo.done) {
            return QColor(255, 0, 0, 70);
        }
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
    if (mManager->updateTodo(todo)) {
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

QModelIndex TodoModel::todoIndex(qint64 id) const
{
    int row;
    for (row = 0; row < mTodo.count(); ++row) {
        if (mTodo.at(row).id == id) {
            return QAbstractTableModel::index(row, 0);
        }
    }

    return QModelIndex();
}

void TodoModel::finishPomodoro(const QModelIndex &index)
{
    if (index.row() < 0 || index.row() >= mTodo.count()) {
        Q_ASSERT_X(false, Q_FUNC_INFO, "Bad Index");
    }

    changePomodoroState(index, Todo::Finished);
}

void TodoModel::voidPomodoro(const QModelIndex &index)
{
    if (index.row() < 0 || index.row() >= mTodo.count()) {
            Q_ASSERT_X(false, Q_FUNC_INFO, "Bad Index");
    }

    changePomodoroState(index, Todo::Void);
}

void TodoModel::addInterruption(const QModelIndex &index, const Todo::Interruption &interruption)
{
    if (index.row() < 0 || index.row() >= mTodo.count()) {
        Q_ASSERT_X(false, Q_FUNC_INFO, "Bad Index");
    }

    Todo &todo = mTodo[index.row()];
    todo.interruptions << interruption;
    const QModelIndex &interruptionIndex = QAbstractTableModel::index(index.row(), Interruption);
    emit dataChanged(interruptionIndex, interruptionIndex);
}

bool TodoModel::canReestimate(const QModelIndex &index) const
{
    if (index.row() < 0 || index.row() >= mTodo.count()) {
        Q_ASSERT_X(false, Q_FUNC_INFO, "Bad Index");
    }

    const Todo &todo = mTodo.at(index.row());
    return todo.reestimation.size() < MaxReestimation;
}

void TodoModel::reestimate(const QModelIndex &index, int reestimation)
{
    if (index.row() < 0 || index.row() >= mTodo.count()) {
        Q_ASSERT_X(false, Q_FUNC_INFO, "Bad Index");
    }

    Todo &todo = mTodo[index.row()];
    todo.reestimation << reestimation;
    for (int i = 0; i < reestimation; ++i) {
        todo.states.append(Todo::OnHold);
    }
    if (mManager->updateTodo(todo)) {
        const QModelIndex &pomdoroIndex = QAbstractTableModel::index(index.row(), Pomodoro);
        emit dataChanged(pomdoroIndex, pomdoroIndex);
    }
}

void TodoModel::taskDone(const QModelIndex &index)
{
    if (index.row() < 0 || index.row() >= mTodo.count()) {
        Q_ASSERT_X(false, Q_FUNC_INFO, "Bad Index");
    }

    Todo &todo = mTodo[index.row()];
    todo.done = true;
    const QModelIndex &topLeft = QAbstractTableModel::index(index.row(), Type);
    const QModelIndex &bottomRight = QAbstractTableModel::index(index.row(), Description);
    emit dataChanged(topLeft, bottomRight);
}

bool TodoModel::isDone(const QModelIndex &index) const
{
    if (index.row() < 0 || index.row() >= mTodo.count()) {
        Q_ASSERT_X(false, Q_FUNC_INFO, "Bad Index");
    }

    return mTodo.at(index.row()).done;
}

bool TodoModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || row + count > mTodo.count()) {
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

void TodoModel::changePomodoroState(const QModelIndex &index, Todo::PomodoroState state)
{
    Todo &todo = mTodo[index.row()];
    todo.states[todo.pomodoro_done] = state;
    ++todo.pomodoro_done;
    if (mManager->updateTodo(todo)) {
        const QModelIndex &pomodoroIndex = QAbstractTableModel::index(index.row(), Pomodoro);
        emit dataChanged(pomodoroIndex, pomodoroIndex);
    }
}
