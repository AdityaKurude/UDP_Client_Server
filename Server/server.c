#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sqlite3.h>
#include<unistd.h>

#define BUFLEN 512  //Max length of buffer
#define IP      "127.0.0.1"
#define SND_PORT 5000   //port to send data
#define REC_PORT 5001   //port on which to listen for incoming data

#define SET_CMD "SetName"
#define GET_CMD "GetName"
#define MISSING_CMD "Missing"

#define DB_NAME "client_names.db"
#define TABLE_NAME "CLIENTS"

char out_name[BUFLEN];

void exit_error(char *s) {
    perror(s);
    exit(1);
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    for(int i = 0; i<argc; i++) {
        if( (0 == strcmp("Output",azColName[i])) && (0 != strcmp("",argv[i]))) {
            strcpy(out_name,argv[i]);
        }
    }
    return 0;
}

void create_database() {

    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;

    // Open database
    rc = sqlite3_open(DB_NAME, &db);

    if( rc ) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return ;
    }
#ifdef DEBUG
    else
        fprintf(stdout, "Opened database successfully\n");
#endif

    // Create SQL query
    sql = "CREATE TABLE IF NOT EXISTS TABLE_NAME("  \
          "FIRST_NAME           TEXT    NOT NULL," \
          "LAST_NAME           TEXT    NOT NULL);";

    // Execute SQL query
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
#ifdef DEBUG
    else
        fprintf(stdout, "Table created successfully\n");
#endif
    // Create SQL query
    sql = "CREATE UNIQUE INDEX IF NOT EXISTS idx_Fname_Lname ON TABLE_NAME ( FIRST_NAME, LAST_NAME ) ;";

    // Execute SQL query
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);

    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
#ifdef DEBUG
    else
        fprintf(stdout, "Unique IDs created successfully\n");
#endif
    sqlite3_close(db);
}

void insert_client(char* str_name) {
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;

    // Split the full name into first name and last name
    char * str_Fname, *str_Lname;
    str_Fname = strtok (str_name," ");
    str_Lname = strtok (NULL," ");

    // Open database
    rc = sqlite3_open(DB_NAME, &db);

    if( rc ) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return ;
    }
#ifdef DEBUG
    else
        fprintf(stderr, "Opened database successfully\n");
#endif
    // Create SQL query
    sql = sqlite3_mprintf("INSERT OR IGNORE INTO TABLE_NAME (FIRST_NAME,LAST_NAME) " \
                          " VALUES ('%q', '%q');", str_Fname, str_Lname);

    // Execute SQL query
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);

    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
#ifdef DEBUG
    else
        fprintf(stdout, "Records created successfully\n");
#endif
    sqlite3_close(db);
}

void get_info(char* str_name) {
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;
    const char* data = "Callback function called";

    // Open database
    rc = sqlite3_open(DB_NAME, &db);

    if( rc ) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return ;
    }
#ifdef DEBUG
    else
        fprintf(stderr, "Opened database successfully\n");
#endif
    // Create SQL query
    sql = sqlite3_mprintf("SELECT *," \
                          " CASE " \
                          " WHEN FIRST_NAME == '%q' OR LAST_NAME == '%q' AND FIRST_NAME == '%q' THEN LAST_NAME " \
                          " WHEN FIRST_NAME == '%q' OR LAST_NAME == '%q' AND LAST_NAME == '%q' THEN FIRST_NAME " \
                          " ELSE '' END as Output  FROM TABLE_NAME ;", str_name, str_name, str_name, str_name, str_name, str_name);

    // Execute SQL query
    rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);

    if( rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
#ifdef DEBUG
    else
        fprintf(stdout, "Operation done successfully\n");
#endif
    sqlite3_close(db);
}

int find_missing_num(char* str_numbers) {
#ifdef DEBUG
    printf ("Splitting string \"%s\" into tokens:\n",str_numbers);
#endif
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
    struct sockaddr_in si_me, si_other, addr;

    int s, slen = sizeof(si_other) , recv_len;
    char buf[BUFLEN];

    //create a UDP socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        exit_error("socket");
    }

    // set structure to zero
    memset((char *) &si_me, 0, sizeof(si_me));

    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(REC_PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(IP);
    addr.sin_port = htons(SND_PORT);

    //bind socket to port
    if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1) {
        exit_error("bind");
    }
    create_database();

    while(1) {
        printf(" Waiting for next command . . . \n");
        fflush(stdout);

        //receive data, a blocking call
        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1) {
            exit_error("recvfrom()");
        }

        char *rec_cmd, *str_name;
        rec_cmd = strtok (buf," :");
        str_name = strtok (NULL,":\n");
#ifdef DEBUG
        //print details of the client and the data received
        printf("Server Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        printf("Data: %s\n" , buf);
        printf(" command detected = %s \n str_name name = %s \n", rec_cmd, str_name);
#endif
        // Compare and find command requested
        if ( 0 == strcmp(rec_cmd, SET_CMD)) {
            // set command requested
            insert_client(str_name);
            // clear the buffer
            memset(buf, 0, BUFLEN);
            strcpy(buf,"Name added Successfully");
        } else if ( 0 == strcmp(rec_cmd, GET_CMD)) {
            memset(out_name, 0, BUFLEN);
            // get command requested
            get_info(str_name);
            // clear the buffer
            memset(buf, 0, BUFLEN);
            strcpy(buf,out_name);
        } else if (0 == strcmp(rec_cmd, MISSING_CMD) ) {
            int num = find_missing_num(str_name);
            printf(" missing number = %d ", num);
            // clear the buffer
            memset(buf, 0, BUFLEN);
            sprintf(buf,"%d",num);
        } else {
            printf(" Error : Invalid command");
            memset(buf, 0, BUFLEN);
            strcpy(buf, "Error : Invalid command");
        }
        //send appropriate reply to the client
        if (sendto(s, buf, recv_len, 0, (struct sockaddr *) &addr,
                sizeof(addr)) < 0) {
            exit_error("sendto()");
        }
        // clear the buffer
        memset(buf, 0, BUFLEN);
    }
    // close the socket
    close(s);
    return 0;
}
