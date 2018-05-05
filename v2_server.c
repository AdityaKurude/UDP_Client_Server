#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFLEN 512  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data

#define SET_CMD "SetName"
#define GET_CMD "GetName"
#define MISSING_CMD "Missing"

#define DB_NAME "client_names.db"
#define TABLE_NAME "CLIENTS"

void exit_error(char *s)
{
    perror(s);
    exit(1);
}

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
    //   sql = "SELECT * from COMPANY";

    /* Create SQL statement */


    sql = sqlite3_mprintf("SELECT *," \
                          " CASE " \
                          " WHEN FIRST_NAME == '%q' OR LAST_NAME == '%q' AND FIRST_NAME == '%q' THEN LAST_NAME " \
                          " WHEN FIRST_NAME == '%q' OR LAST_NAME == '%q' AND LAST_NAME == '%q' THEN FIRST_NAME " \
                          " ELSE '' END as Output  FROM TABLE_NAME ;", str_name, str_name, str_name, str_name, str_name, str_name);


    //        sql = sqlite3_mprintf("SELECT * FROM TABLE_NAME" \
    //                          " WHERE FIRST_NAME == '%q' OR LAST_NAME == '%q'  ;", str_name, str_name);


    //        sql = sqlite3_mprintf(        " SELECT * " \
    //                                      " FROM TABLE_NAME " \
    //                                      " WHERE EXISTS (SELECT * " \
    //                                                    " FROM TABLE_NAME " \
    //                                                    " WHERE FIRST_NAME == 'Dora' OR LAST_NAME == 'Dora'); ");

    // gives all names as output
    //    sql = sqlite3_mprintf("SELECT FIRST_NAME,LAST_NAME," \
    //                          "CASE " \
    //                          " WHEN FIRST_NAME IS NOT NULL AND FIRST_NAME == '%q' THEN LAST_NAME " \
    //                          " WHEN LAST_NAME IS NOT NULL AND LAST_NAME == '%q' THEN FIRST_NAME " \
    //                          " ELSE '' END as Output FROM TABLE_NAME;", str_name);

    //    sql = sqlite3_mprintf("SELECT FIRST_NAME,LAST_NAME, CASE WHEN FIRST_NAME IS NOT NULL AND FIRST_NAME == '%q' THEN 'NAME present' ELSE 'wahhhhhhhhhhhh Name NULL' END as LAST_NAME FROM TABLE_NAME;", str_name);

    //    sql = "SELECT FIRST_NAME,LAST_NAME CASE WHEN FIRST_NAME IS NOT NULL AND FIRST_NAME == '%q' THEN 'NAME present' ELSE 'wahhhhhhhhhhhh Name NULL' END as NAME FROM COMPANY";


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

int find_missing_num(char* str_numbers) {
    printf ("Splitting string \"%s\" into tokens:\n",str_numbers);
    str_numbers = strtok (str_numbers,",");
    int sum = 0;
    while (str_numbers != NULL) {
        sum += atoi(str_numbers);
        str_numbers = strtok (NULL, ",");
    }

    return (45 - sum);
}

int main(void)
{
    struct sockaddr_in si_me, si_other;

    int s, i, slen = sizeof(si_other) , recv_len;
    char buf[BUFLEN];

    //create a UDP socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        exit_error("socket");
    }

    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));
    memset(buf, 0, BUFLEN);

    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind socket to port
    if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        exit_error("bind");
    }

    create_database();

    //keep listening for data
    while(1)
    {
        printf("Waiting for data... \n");
        fflush(stdout);

        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
        {
            exit_error("recvfrom()");
        }

        //print details of the client/peer and the data received
        printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        printf("Data: %s\n" , buf);


        char *rec_cmd, *str_name;
        printf ("Splitting string %s \n",buf);
        rec_cmd = strtok (buf," :");
        str_name = strtok (NULL,":\n");


        printf(" command detected = %s \n full name = %s \n", rec_cmd, str_name);

        /* Compare the two strings provided */
        if ( 0 == strcmp(rec_cmd, SET_CMD)) {
            // set command requested
            insert_client(str_name);

        } else if ( 0 == strcmp(rec_cmd, GET_CMD)) {
            // get command requested
            get_info(str_name);

        } else if (0 == strcmp(rec_cmd, MISSING_CMD) ) {
            int num = find_missing_num(str_name);
            printf(" missing number = %d ", num);
        }
        else {
            printf(" Error : Invalid command");
            return 1;
        }


        //now reply the client with the same data
        if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1)
        {
            exit_error("sendto()");
        }
        // clear the data received in previous packet
        memset(buf, 0, BUFLEN);

    }

    close(s);
    return 0;
}
