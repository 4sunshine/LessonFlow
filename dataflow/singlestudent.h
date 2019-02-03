#ifndef SINGLESTUDENT_H
#define SINGLESTUDENT_H

#include <QString>

class SingleStudent
{

public:
    //ORDER MEANS STUDENT ID IN CLASS
    SingleStudent(const int &order,const QString &name, const QString &surname,
              const QString &pluses, const QString &average,
              const QString &avatarName, const bool &status, const bool &ismain);
//![0]
    int order() const;
    QString name() const;
    QString surname() const;
    QString pluses() const;
    QString average() const;
    QString avatar() const;
    bool status() const;
    bool ismain() const;

private:
    int m_order;
    QString m_name;
    QString m_surname;
    QString m_pluses;
    QString m_average;
    QString m_avatarName;
    bool m_status;
    bool m_ismain;
//![1]
};

#endif // SINGLESTUDENT_H
