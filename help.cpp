#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

extern "C" {
    char* generate_help() {
        std::ifstream file("./pandas.html");
        if (!file) {
            const char *error_message = "Error: Could not open pandas.html";
            char *result = (char *)malloc(strlen(error_message) + 1);
            strcpy(result, error_message);
            return result;
        }

        std::ostringstream content;
        content << file.rdbuf();
        file.close();

        std::string content_str = content.str();
        char *result = (char *)malloc(content_str.length() + 1);
        strcpy(result, content_str.c_str());

        return result; // The caller is responsible for freeing this memory
    }
}
