#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h> 
#include <string.h>

#define DB_NAME "client_names.db"
#define TABLE_NAME "CLIENTS"

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    int i;
    for(i = 0; i<argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

int create_database() {

    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;

    /* Open database */
    rc = sqlite3_open(DB_NAME, &db);

    if( rc ) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return(0);
    } else {
        fprintf(stdout, "Opened database successfully\n");
    }

    /* Create SQL statement */
    sql = "CREATE TABLE IF NOT EXISTS TABLE_NAME("  \
          "FIRST_NAME           TEXT    NOT NULL," \
          "LAST_NAME           TEXT    NOT NULL);";

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Table created successfully\n");
    }

    /* Create SQL statement */
    sql = "CREATE UNIQUE INDEX IF NOT EXISTS idx_Fname_Lname ON TABLE_NAME ( FIRST_NAME, LAST_NAME ) ;";


    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Unique IDs created successfully\n");
    }

    sqlite3_close(db);

}

int insert_client(char* str_name) {
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;

    // Split the full name into first name and last name
    char * str_Fname, *str_Lname;
    str_Fname = strtok (str_name," ");
    str_Lname = strtok (NULL," ");

    /* Open database */
    rc = sqlite3_open(DB_NAME, &db);

    if( rc ) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return(0);
    } else {
        fprintf(stderr, "Opened database successfully\n");
    }

    /* Create SQL statement */
    /*  sql = "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) "  \
         "VALUES (1, 'Paul', 32, 'California', 20000.00 ); " \
         "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) "  \
         "VALUES (2, 'Allen', 25, 'Texas', 15000.00 ); "     \
         "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)" \
         "VALUES (3, 'Teddy', 23, 'Norway', 20000.00 );" \
         "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)" \
         "VALUES (4, 'Mark', 25, 'Rich-Mond ', 65000.00 );";
    */



    /* Create SQL statement */
    /*  sql = "INSERT OR IGNORE INTO TABLE_NAME (FIRST_NAME,LAST_NAME) "  \
         "VALUES ('%q' , '%q');",str_Fname, str_Lname;
*/
    sql = sqlite3_mprintf("INSERT OR IGNORE INTO TABLE_NAME (FIRST_NAME,LAST_NAME) " \
                          " VALUES ('%q', '%q');", str_Fname, str_Lname);

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);



    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Records created successfully\n");
    }
    sqlite3_close(db);

}

void get_info(char* str_name) {
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;
    const char* data = "Callback function called";

    /* Open database */
    rc = sqlite3_open(DB_NAME, &db);

    if( rc ) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        //       return(0);
    } else {
        fprintf(stderr, "Opened database successfully\n");
    }

    /* Create SQL statement */
    sql = sqlite3_mprintf("SELECT *," \
                          " CASE " \
                          " WHEN FIRST_NAME == '%q' OR LAST_NAME == '%q' AND FIRST_NAME == '%q' THEN LAST_NAME " \
                          " WHEN FIRST_NAME == '%q' OR LAST_NAME == '%q' AND LAST_NAME == '%q' THEN FIRST_NAME " \
                          " ELSE '' END as Output  FROM TABLE_NAME ;", str_name, str_name, str_name, str_name, str_name, str_name);

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);

    if( rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Operation done successfully\n");
    }
    sqlite3_close(db);
}

int main(int argc, char* argv[]) {

    char str_fname[] ="Dora Kosti";

    create_database();

    insert_client(str_fname);

    char str_lname[] ="Dora";

    get_info(str_lname);

    return 0;
}
