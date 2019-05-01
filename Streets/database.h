#ifndef DATABASE_H
#define DATABASE_H

#include <QtSql>

class Database
{
public:
    Database();
    void CreateTables();
private:
    QSqlDatabase db;
    QSqlQuery query;
};

#endif // DATABASE_H
