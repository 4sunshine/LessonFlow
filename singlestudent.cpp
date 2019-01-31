#include "singlestudent.h"

SingleStudent::SingleStudent(const int &order, const QString &name, const QString &surname,
                             const QString &pluses, const QString &average,
                             const QString &avatarName, const bool &status):
    m_order(order),
     m_name(name), m_surname(surname), m_pluses(pluses), m_average(average),
      m_avatarName(avatarName), m_status(status)
{
}

QString SingleStudent::name() const
{
    return m_name;
}

QString SingleStudent::surname() const
{
    return m_surname;
}

QString SingleStudent::pluses() const
{
    return m_pluses;
}

QString SingleStudent::average() const
{
    return m_average;
}

QString SingleStudent::avatar() const
{
    return m_avatarName;
}

bool SingleStudent::status() const
{
    return m_status;
}

int SingleStudent::order() const
{
    return m_order;
}
