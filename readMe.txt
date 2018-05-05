This project demonstrate some basic communication using UDP between a client and server located on local machine. Server creates a lightweight database to store the names provided by client using sqlite3, hence you should install sqlite on your machine before testing. Following command can be used to install sqlite3 on ubuntu machine.

sudo apt-get install sqlite3 libsqlite3-dev

Folder Server and Client contains their respective makefiles for easy compilation. Release build can be done using simply "make", however debug build can be obtained using "make debug" command.


Following is a short description of the task which can be performed with this client server based system.


1. Purpose "SetName" and "GetName":

The server helps to maintain a list of names (first name and last name). If it gets a message "SetName:Max Goldman" from a client, it saves the name to its storage. A client may request a corresponding first name by sending a last name (e.g. "GetName:Goldman"). The server then reply with a message calling the corresponding first name "Max" and vice a versa. It only handles name consisting of one first and one last name. If there exist the same name then new entry is ignored.The storage is persistent but database can be deleted with "make clean" command.

Example message flow:
1. Client sends "SetName:Gola Pan"
2. Client sends "GetName:Pan"
3. Server sends "Gola"


2. Purpose "Missing":

The server identifies a missing number in a row of 8 random non-repeating numbers between 1 and 9. For example, in the following sequence of 8 numbers, one number is missing "1,4,2,5,3,6,7,9".  In this example number 8 is missing. The server replies with that missing number. The list of numbers received from a client is comma separated.

Example message flow:
1. Client sends: "Missing:4,9,2,3,8,1,5,7"
2. Server sends: "6"






