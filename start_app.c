#include <stdio.h>
#include "mapping.h"
#include "listen.h"
#include <stdlib.h>
#include <string.h>
/*
    This is the start of the server application
*/


int main(){

    //lets start by getting data uri and respective functions
    //lets keep that data in the dynamic memeory 
    file_info * fi = count_lines_n_size_route_map("./routes.map");
    char ** data = data_from_routes_file(fi);

    for (int j=0;j<fi->number_of_lines;j++){
        printf("%d %s\n",j,data[j]);
    }
    listen_for_rq(data);
    //freeing data at the end
    for (int i=0;i<fi->number_of_lines;i++){
        free(data[i]);
    }
    free(data);
    //free file info pointer
    free(fi);
    printf("-----------------------END-----------------------\n");
    return 0;
}