#ifndef MAPPING_H
#define MAPPING_H

typedef struct {int number_of_lines;int largest_line_size;} file_info;
// Function declaration
file_info * count_lines_n_size_route_map(char * file_loc);
char ** data_from_routes_file(file_info * f_i);

#endif
