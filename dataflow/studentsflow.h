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
        IsmainRole
    };

    StudentsFlow(QObject *parent = nullptr);

    void addStudent(const SingleStudent &student);

    void removeStudent(const int index);

    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    bool setData(const QModelIndex & index, const QVariant &value, int role);

    void setIsOn(int i);

    void removeAll(); //REMOVE ALL NOT MAIN STUDENTS
// DISCUSSION MODE MUST BE IMPLEMENTED WHEN SEVERAL STUDENTS ARE ACTIVE
    void activeStudent(int id);

    void setMain(int id);

    void setPM(QString plumin, int id);

    void setAverage(QString average, int id);

    int getFirst(); // RETURN ID OF FIRST STUDENT

    int getLast(); // RETURN ID OF LAST MAIN STUDENT

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    QList<SingleStudent> m_students;
    int findById(int id); // RETURNS INDEX POSITION OF STUDENT WITH ID
    int getMainCount(); // RETURNS COUNT OF MAIN STUDENTS

};

#endif // STUDENTSFLOW_H
