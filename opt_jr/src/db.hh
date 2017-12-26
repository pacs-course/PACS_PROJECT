#ifndef SRC_DB_H_
#define SRC_DB_H_
#include <my_global.h>
#include <mysql.h>
#include "optjrParameters.hh"



/*
	-Here are declared the functions to manage databases
 */


/**
  Standard error procedure for DB operations
*/
void DBerror(MYSQL *conn, char * msg);

/**
  Execute SQL statement
*/
MYSQL_ROW executeSQL(MYSQL *conn, char *statement, optJrParameters par);

/**
  Open a DB connection
 */

 MYSQL * DBopen(char * host, char * port, char * login, char * passw, char *dbName);

/**
  Close DB connection
 */
void DBclose(MYSQL *conn);




#endif /* SRC_DB_H_ */
