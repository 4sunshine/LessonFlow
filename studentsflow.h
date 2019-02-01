#ifndef STUDENTSFLOW_H
#define STUDENTSFLOW_H

#include <QAbstractListModel>
#include "singlestudent.h"

class StudentsFlow : public QAbstractListModel
{
    Q_OBJECT
public:
    enum MyStudentRoles {
        OrderRole = Qt::UserRole + 1,
        NameRole,
        SurnameRole,
        PlusesRole,
        AverageRole,
        AvatarRole,
        StatusRole,
        IsMainRole
    };

    StudentsFlow(QObject *parent = nullptr);

    void addStudent(const SingleStudent &student);

    void removeStudent(const int index);

    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    bool setData(const QModelIndex & index, const QVariant &value, int role);

    void changeValue(int i);

    void setIsOn(int i);

    void removeAll();
// DISCUSSION MODE MUST BE IMPLEMENTED WHEN SEVERAL STUDENTS ARE ACTIVE
    void activeStudent(int id);

    void setMain();

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    QList<SingleStudent> m_students;

};

#endif // STUDENTSFLOW_H
