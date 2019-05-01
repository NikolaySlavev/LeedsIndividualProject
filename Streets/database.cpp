#include "database.h"
#include <iostream>

using namespace std;

Database::Database() {
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("test");
    db.setUserName("4aspic");
    db.setPassword("");
    bool ok = db.open();
    std::cout << ok << std::endl;

    query = *new QSqlQuery;
    query.exec("SELECT * FROM test1");
    while (query.next()) {
           QString name = query.value(1).toString();
           //int salary = query.value(1).toInt();
           qDebug() << name << endl;
       }
}

void Database::CreateTables() {
    query.exec("CREATE TABLE IF NOT EXISTS node ( \
               id int unsigned auto_increment primary key, \
               x float, \
               y float, \
               z float  \
               );" );

    query.exec("CREATE TABLE IF NOT EXISTS node_offset ( \
               id int unsigned auto_increment primary key, \
               x float, \
               y float, \
               z float  \
           );");

    query.exec("CREATE TABLE IF NOT EXISTS edge_offset ( \
               id int unsigned auto_increment primary key, \
               start_node int unsigned, \
               end_node int unsigned, \
               FOREIGN KEY (start_node) REFERENCES node_offset(id), \
               FOREIGN KEY (end_node) REFERENCES node_offset(id) \
           );");

    query.exec("CREATE TABLE IF NOT EXISTS edge_axis ( \
               id int unsigned auto_increment primary key, \
               start_node int unsigned, \
               end_node int unsigned, \
               offset_up int unsigned, \
               offset_down int unsigned, \
               FOREIGN KEY (start_node) REFERENCES node(id), \
               FOREIGN KEY (end_node) REFERENCES node(id), \
               FOREIGN KEY (offset_up) REFERENCES edge_offset(id), \
               FOREIGN KEY (offset_down) REFERENCES edge_offset(id), \
               closest_intersection float \
           );");
}



