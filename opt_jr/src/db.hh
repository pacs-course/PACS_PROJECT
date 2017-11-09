#ifndef SRC_DB_H_
#define SRC_DB_H_
#include <my_global.h>
#include <mysql.h>
#include "optjrParameters.hh"


/*
  -Questo file è mantenuto sostanzialmente inalterato rispetto alla versione in C
  -Qui sono dichiarate le funzioni  per gestire database;
*/



void DBerror(MYSQL *conn, char * msg);
MYSQL_ROW executeSQL(MYSQL *conn, char *statement, optJrParameters par);
MYSQL * DBopen(char * host, char * login, char * passw, char *dbName);

void DBclose(MYSQL *conn);




#endif /* SRC_DB_H_ */
