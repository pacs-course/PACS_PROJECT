#ifndef SRC_DB_H_
#define SRC_DB_H_
#include <my_global.h>
#include <mysql.h>
#include "optjrParameters.hh"



/*
	-This file is not substantially changed from original C version
	-Here are declared the functions to manage databases
 */


/**
  Standard error procedure for DB operations
  (not substantially changed from original C version)
*/
void DBerror(MYSQL *conn, char * msg);

/**
  Execute SQL statement
  (not substantially changed from original C version)
*/
MYSQL_ROW executeSQL(MYSQL *conn, char *statement, optJrParameters par);

/**
  Open a DB connection
 (not substantially changed from original C version)
 */

 MYSQL * DBopen(char * host, char * port, char * login, char * passw, char *dbName);

/**
  Close DB connection
 (not substantially changed from original C version)
 */
void DBclose(MYSQL *conn);




#endif /* SRC_DB_H_ */
