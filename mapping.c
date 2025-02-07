#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
    This is for parsing routes.map file and identifying which is what ?
*/

typedef struct {
    int number_of_lines;
    int largest_line_size;
} file_info ;

//for counting number of lines and largest sized line string
file_info * count_lines_n_size_route_map(char * file_loc){

    file_info * f_i = NULL;
    f_i = malloc(sizeof(file_info));
    FILE * file = fopen(file_loc,"r");
    if (file==NULL){
        perror("routes file couldn't be opened.....\n");
        return NULL;
    }
    int lines_count = 0;
    int prev_large = 0;
    int largest_line_size = 0;
    char ch;

    while(1){
        ch = fgetc(file);
        if (ch=='\n' || ch==EOF){
            lines_count++;
            if (prev_large<largest_line_size){
                prev_large = largest_line_size;
                largest_line_size = 0;
            }
            if (ch==EOF){
                largest_line_size = prev_large;
                break;
            }
        }else{
            largest_line_size++;
        }
    }
    f_i->largest_line_size = largest_line_size;
    f_i->number_of_lines = lines_count;
    fclose(file);
    return f_i;
}

//returns a 2d array with data of strings in the routes.map file

char ** data_from_routes_file(file_info * f_i){
    FILE * file = fopen("./routes.map","r");
    if (file==NULL){
        perror("Error for opening routes....\n");
        return NULL;
    }

    char * line  = NULL;
    size_t len = 0;
    ssize_t read ;

    if (f_i==NULL){
        perror("Something went wrong....\n");
        return NULL;
    }
    //data storing in md-array
    int number_of_lines = f_i->number_of_lines;
    int largest_line_size = f_i->largest_line_size+1; // extra for '\0'
    char** data = malloc(number_of_lines*sizeof(char *));
    int count_line =0;
    while((read = getline(&line,&len,file))!=-1){
        if (line[read - 1] == '\n') {
            line[read - 1] = '\0';  // Remove the newline character
        }
        data[count_line] = malloc(largest_line_size*sizeof(char));
        strcpy(data[count_line],line);
        data[count_line][largest_line_size-1] = '\0';
        count_line++;
    }
    free(line);
    fclose(file);
    return data;
}