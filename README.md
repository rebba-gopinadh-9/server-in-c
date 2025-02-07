# server-in-c
this is a project to demonstrate a simple extendable sever in c and cpp


This is simple demonstration of server that is written in c

first it opens a tcp connection on port **9000** and listens for incoming requests

then it parses the request for different data like :
path 
type 
content-type
and body etc,

next it matches the path with the function mentioned in the **routes.map** file 
                                              ( which is sorta cofig file for matching requests to functions)

the matched function is searched in the **libmylib.so** file which is produced from **help.cpp** file

this contains the matched **generate_help** function which returns the file **pandas.html**

The idea is the main c files form the core server in which we can deploy different backend .so files written in cpp and match with routes.map file for different requests
