#include <stdio.h>
#include <string.h>
#include <string>
#include "db.hh"


/*
	-Here are defined the functions to manage databases
 */



/*
 * Standard error procedure for DB operations
 */


void DBerror(MYSQL *conn, char * msg)
{
	printf("DBError: %s %s\n", msg, mysql_error(conn));
	mysql_close(conn);
	exit(-1);
}


MYSQL_ROW executeSQL(MYSQL *conn, char *statement, optJrParameters par)
{
	MYSQL_RES *result;
	char error[256];
	std::string debugMsg;
	if (mysql_query(conn, statement))
	{
		char error[512];
		sprintf(error,"SQL failure executeSQL on %s", statement);
		DBerror(conn, error);
	}

	result = mysql_store_result(conn);
	debugMsg="statement :"+ std::string(statement) + "\n"; par.debugMessage(debugMsg);
	if (result == NULL)
	{
		sprintf(error, "Failure: ExecuteSQL: statement was %s\n", statement);
		DBerror(conn, error);
		return NULL;
	}
	else
		return(mysql_fetch_row(result));

}




 /*
  * Open a DB connection
  */

 MYSQL * DBopen(char * host, char * port, char * login, char * passw, char *dbName)
 {


 MYSQL *con = mysql_init(NULL);

   if (con == NULL)
   {
       fprintf(stderr, "DBError: %s\n", mysql_error(con));
       exit(1);
   }

   if (mysql_real_connect(con, host, login, passw,
           dbName, atoi(port), NULL, 0) == NULL) DBerror(con, (char*)"mysql_real_connect");

   return con;
 }




/*
 * Close the Db connection
 */

void DBclose(MYSQL *conn)
{
	mysql_close(conn);
}
