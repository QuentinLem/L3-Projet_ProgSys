#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "../include/map.h"
#include "../include/error.h"

#define BUFFER_MAX_SIZE 300

#define LINE_WIDTH 1
#define LINE_HEIGHT 2
#define LINE_OBJECTS 3

int is_map_file(char *filename){
    //not implemented
    return 1;
}

FILE *open_file_RD(char *filename){
    FILE *fp = fopen(filename, "r");
    if(fp == NULL){
        exit_with_error("'maputil' failed on open file '%s' (read mode)\n", filename);
    }
}

void read_line_n(FILE *fp, char *buffer, int line){
    for(int i = 0; i < line; i++) {
        fgets(buffer, BUFFER_MAX_SIZE, fp);
    }
    rewind(fp);
}

void print_maputil_error(){
    printf("maputil ERROR\nUsage: maputil <filename(.map)> --[OPTIONS]\nTry 'maputil --help' to display commands\n");
}

void print_maputil_help(){
    printf("\nmaputil HELP\nUsage: maputil <filename> --[OPTIONS]\n<filename> must be a .map file\n\nOPTIONS:\n--help\n<filename> --getwidth\n<filename> --getheight\n<filename> --getobjects\n<filename> --getinfo\n\n<filename> --setwidth\n<filename> --setheight (not implemented)\n\n");
}

void print_maputil_getwidth(FILE *fp){
    int line_in_file = LINE_WIDTH;
    char buffer[BUFFER_MAX_SIZE];
    read_line_n(fp, buffer, line_in_file);
    printf("MAP WIDTH: %s", buffer);
}

void print_maputil_getheight(FILE *fp){
    int line_in_file = LINE_HEIGHT;
    char buffer[BUFFER_MAX_SIZE];
    read_line_n(fp, buffer, line_in_file);
    printf("MAP HEIGHT: %s", buffer);
}

void print_maputil_getobjects(FILE *fp){
    int line_in_file = LINE_OBJECTS;
    char buffer[BUFFER_MAX_SIZE];
    read_line_n(fp, buffer, line_in_file);
    printf("MAP OBJECTS: %s", buffer);
}

void print_maputil_getinfo(FILE *fp){
    printf("-- MAP info --\n");
    print_maputil_getwidth(fp);
    print_maputil_getheight(fp);
    print_maputil_getobjects(fp);
}

void print_maputil_setwidth(){
    exit_with_error ("maputil --setheight option is not yet implemented\n");
}

void print_maputil_setheight(){
    exit_with_error ("maputil --setheight option is not yet implemented\n");
}

int main(int argc, char **argv){
    
    FILE *fp;
    char *file = argv[1];
    char *option = argv[2];
    char *option2 = argv[3];

    if(argc == 2 && !strcmp(argv[1], "--help")) {
        print_maputil_help();

    } else if(argc == 3 && is_map_file(file)) {

        fp = open_file_RD(file);

        if(!strcmp(option, "--getwidth")) {
            print_maputil_getwidth(fp);
        } else if(!strcmp(option, "--getheight")) {
            print_maputil_getheight(fp);
        } else if(!strcmp(option, "--getobjects")) {
            print_maputil_getobjects(fp);
        } else if(!strcmp(option, "--getinfo")) {
            print_maputil_getinfo(fp);
        } else {
            print_maputil_error();
        }

        fclose(fp);

    } else if(argc == 4 && is_map_file(file)) {

        fp = open_file_RD(file);

        if(!strcmp(option, "--setwidth")){
            print_maputil_setwidth();
        } else if(!strcmp(option, "--setheight")){
            print_maputil_setheight();
        } else {
            print_maputil_error();    
        }

        fclose(fp);

    } else {
        print_maputil_error();
    }
}