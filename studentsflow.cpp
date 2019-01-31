#include "studentsflow.h"
#include <QDebug>

StudentsFlow::StudentsFlow(QObject *parent)
    : QAbstractListModel(parent)
{
}

void StudentsFlow::addStudent(const SingleStudent &myStudent)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_students << myStudent;
    endInsertRows();
}

void StudentsFlow::removeStudent(const int ind)
{
    int i = 0;
    while((m_students[i].order() != ind) && (i < rowCount()))
    {
        i++;
    }

    if (i < rowCount())
    {
        beginRemoveRows(QModelIndex(),i,i);
        m_students.removeAt(i);
        endRemoveRows();
    }

}

int StudentsFlow::rowCount(const QModelIndex & parent) const {
    Q_UNUSED(parent);
    return m_students.count();
}

QVariant StudentsFlow::data(const QModelIndex & index, int role) const {
    if (index.row() < 0 || index.row() >= m_students.count())
        return QVariant();

    const SingleStudent &myStudent = m_students[index.row()];
    if (role == NameRole)
        return myStudent.name();
    else if (role == SurnameRole)
        return myStudent.surname();
    else if (role == PlusesRole)
        return myStudent.pluses();
    else if (role == AverageRole)
        return myStudent.average();
    else if (role == AvatarRole)
        return myStudent.avatar();
    else if (role == StatusRole)
        return myStudent.status();
    return QVariant();
}

//![0]
QHash<int, QByteArray> StudentsFlow::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[OrderRole] = "order";
    roles[NameRole] = "name";
    roles[SurnameRole] = "surname";
    roles[AverageRole] = "average";
    roles[PlusesRole] = "pluses";
    roles[AvatarRole] = "avatar";
    roles[StatusRole] = "status";
    return roles;
}
//![0]

bool StudentsFlow::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.row() >= 0 && index.row() < rowCount()
        && (role==StatusRole))//(role == Qt::EditRole || role == Qt::DisplayRole))
    {
        SingleStudent local = m_students[index.row()];
        m_students.replace(index.row(), SingleStudent(local.order(),local.name(),local.surname(),
                                                      local.pluses(),
                                                      local.average(),
                                                      local.avatar(),value.toBool()));
        emit dataChanged(index, index, {role});
        return true;
    }
    return false;
}

void StudentsFlow::changeValue(int i)
{

    setData(this->index(i), true, StatusRole);

}

void StudentsFlow::setIsOn(int i)
{
    setData(this->index(i), true, StatusRole);
}

void StudentsFlow::removeAll()
{
    int i = rowCount();
    while (rowCount() > 0){
        beginRemoveRows(QModelIndex(),i,i);
        m_students.removeAt(i);
        endRemoveRows();
        i--;
    }
}
