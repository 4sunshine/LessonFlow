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
    roles[IsMainRole] = "isMain";
    return roles;
}
//![0]

bool StudentsFlow::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.row() >= 0 && index.row() < rowCount())
    {
        SingleStudent local = m_students[index.row()];
        switch (role) {
        case StatusRole:

            m_students.replace(index.row(), SingleStudent(local.order(),local.name(),
                                                      local.surname(),
                                                      local.pluses(),
                                                      local.average(),
                                                      local.avatar(),value.toBool(),
                                                      local.isMain()));
            emit dataChanged(index, index, {role});
            return true;

        case IsMainRole:

            m_students.replace(index.row(), SingleStudent(local.order(),local.name(),
                                                      local.surname(),
                                                      local.pluses(),
                                                      local.average(),
                                                      local.avatar(),local.status(),
                                                      value.toBool()));
            qInfo() << "MAIN CHANGED";
            qInfo() << m_students[0].isMain();
            emit dataChanged(index, index, {role});
            return true;

        default:
            return false;
        }
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

void StudentsFlow::setMain()
{
    setData(this->index(0), true, IsMainRole);
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

void StudentsFlow::activeStudent(int id)
{
    qInfo() << id;
    int i = rowCount() - 1 ; //INITIAL I MUST BE ROWCOUNT-1 <INDEX OUT OF RANGE>
    while (rowCount() > 1){
        if (m_students[i].order() != id)
        {
            qInfo() << i;
            beginRemoveRows(QModelIndex(),i,i);
            m_students.removeAt(i);
            endRemoveRows();
        }
        i--;
    }
    qInfo() << "SET MAIN";
    setMain();
}
