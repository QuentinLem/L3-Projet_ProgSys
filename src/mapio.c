#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "map.h"
#include "error.h"

#ifdef PADAWAN

#define BUFFER_MAX_SIZE 3096

FILE *load_open_file(char *filename, char *mode){
    FILE *fp = fopen(filename, mode);
    if(fp == NULL){
        exit_with_error("'maputil' failed on open file '%s' (%s mode)\n", filename, mode);
    }
    return fp;
}

void read_line_n(FILE *fp, char *buffer, int line){
    rewind(fp);
    for(int i = 0; i < line; i++) {
        if(!fgets(buffer, BUFFER_MAX_SIZE, fp)){
          exit_with_error("Error on load: 'read_line_n'\n");
        }
    }
}

void read_next_line(FILE *fp, char *buffer){
  if(!fgets(buffer, BUFFER_MAX_SIZE, fp)){
    exit_with_error("Error on load: 'read_next_line'\n");
  }
}

void natural_write(int fd, char str[BUFFER_MAX_SIZE]) {
  if(write(fd, str, sizeof(char)*strlen(str)) == -1){
      exit_with_error ("Map save failed: 'write'' function\n");
    }
}

int init_saving_file(char *filename) {
  int fd = open(filename, O_WRONLY|O_CREAT, 0666);
  if(fd == -1){
    exit_with_error("Map_save failed on open save_file\n");
  }
  if(ftruncate(fd,0) == -1){
    exit_with_error("Map_save failed on truncate save_file\n");
  }
  return fd;
}

int isValueInArray(int val, int *array, int array_size){
  for(int i = 0; i < array_size; i++){
    if(array[i] == val){
      return 1;
    }
  }
  return 0;
}

int save_obj_on_map(int *array, int obj_val, int iterator) {
  // si l'objet n'est as encore dans la liste
  if(!isValueInArray(obj_val, array, iterator)) {
    // stockage de l'ID du nouvel objet decouvert
    array[iterator] = obj_val;
    return 1;
  }
  return 0;
}

void print_map_status(int fd, int map_width, int map_height, int map_nb_obj) {
  char buffer[BUFFER_MAX_SIZE];
  sprintf(buffer, "%d\n%d\n%d\n", map_width, map_height, map_nb_obj);
  natural_write(fd, buffer);
}

void print_map_matrix(int fd, int map_width, int map_height) {
  char buffer[BUFFER_MAX_SIZE];
  // write de la matrice de la map (bas->haut, gauche->droite)
  for(int i = map_height-1; i >= 0; i--){
    for(int j = 0; j < map_width; j++){
      sprintf(buffer, "%d ", map_get(j,i));
      natural_write(fd, buffer);
    }
    natural_write(fd, "\n");
  }
}

void get_obj_properties(int id_obj, char* buffer){

  sprintf(buffer, "%s %d %d", map_get_name(id_obj), map_get_frames(id_obj), map_get_solidity(id_obj));

  if(map_is_destructible(id_obj)){
    strcat(buffer, " 4");
  }
  if(map_is_collectible(id_obj)){
    strcat(buffer, " 8");
  }
  if(map_is_generator(id_obj)){
    strcat(buffer, " 16");
  }

  strcat(buffer, "\n");
}

void print_map_objects(int fd, int nb_obj){
  char obj_properties[BUFFER_MAX_SIZE];
  int curr_obj;
  
  for(int i = 0; i < nb_obj; i++){
    curr_obj = i;
    get_obj_properties(curr_obj, obj_properties);
    natural_write(fd, obj_properties);
  }
}

void map_new (unsigned width, unsigned height) {
  map_allocate (width, height);

  for (int x = 0; x < width; x++)
    map_set (x, height - 1, 0); // Ground

  for (int y = 0; y < height - 1; y++) {
    map_set (0, y, 1); // Wall
    map_set (width - 1, y, 1); // Wall
  }

  map_object_begin (8);

  // Texture pour le sol
  map_object_add ("images/ground.png", 1, MAP_OBJECT_SOLID);
  // Mur
  map_object_add ("images/wall.png", 1, MAP_OBJECT_SOLID);
  // Gazon
  map_object_add ("images/grass.png", 1, MAP_OBJECT_SEMI_SOLID);
  // Marbre
  map_object_add ("images/marble.png", 1, MAP_OBJECT_SOLID | MAP_OBJECT_DESTRUCTIBLE);
  // Marbre2
  map_object_add ("images/marble2.png", 1, MAP_OBJECT_SOLID | MAP_OBJECT_DESTRUCTIBLE);
  // Fleur1
  map_object_add("images/flower.png", 1, MAP_OBJECT_AIR);
  //Fleur2
  map_object_add("images/flower2.png", 1, MAP_OBJECT_AIR);
  //Piece map
  map_object_add("images/coin.png", 20, MAP_OBJECT_AIR | MAP_OBJECT_COLLECTIBLE);

  map_object_end ();

}

void map_save (char *filename) {
  int width = map_width();
  int height = map_height();
  int nb_obj = map_objects();

  int fd = init_saving_file(filename);
  print_map_status(fd, width, height, nb_obj);
  print_map_objects(fd, nb_obj);
  print_map_matrix(fd, width, height);
  
  close(fd);
  fprintf(stdout, "Map saved on: %s\n", filename);
}

void load_map_matrix(FILE *fp, char *buffer, char *separator,int height, int width, int line_end_list_obj){
  char *token;
  read_line_n(fp, buffer, line_end_list_obj);
  for(int i = height-1; i >= 0; i--){
    read_next_line(fp, buffer);
    token = strtok(buffer, separator);
    for(int j = 0; j < width; j++){
      map_set(j, i, atoi(token));
      token = strtok(NULL, separator);
    }
  }
}

void load_map_object(FILE *fp, char *buffer, char *separator, int line){
  char *token;
  char *obj_name;
  unsigned int obj_frames;
  unsigned int obj_type;
  read_line_n(fp, buffer, line);
  
  token = strtok(buffer, separator);
  obj_name = token;

  token = strtok(NULL, separator);
  obj_frames = atoi(token);
  
  token = strtok(NULL, separator);
  obj_type = atoi(token);

  while(token != NULL) {
    token = strtok(NULL, separator);
    if(token != NULL) {
      obj_type = obj_type | atoi(token);
    }
  }
  map_object_add(obj_name, obj_frames, obj_type);
}

void load_map_objects(FILE *fp, char *buffer, char *separator, int nb_obj){
  map_object_begin(nb_obj);
  for(int i = 0; i < nb_obj; i++){
    load_map_object(fp, buffer, separator, 4+i);
  }
  map_object_end();
}

void map_load (char *filename) {
  
  FILE *fp = load_open_file(filename, "r");
  char buffer[BUFFER_MAX_SIZE];
  char *separator = " ";

  int width = atoi(fgets(buffer, BUFFER_MAX_SIZE, fp));
  int height = atoi(fgets(buffer, BUFFER_MAX_SIZE, fp));
  int nb_obj = atoi(fgets(buffer, BUFFER_MAX_SIZE, fp));

  int line_end_list_obj = 3+nb_obj;  
  
  map_allocate(width, height);
  load_map_matrix(fp, buffer, separator, height, width, line_end_list_obj);
  load_map_objects(fp, buffer, separator, nb_obj);

  fclose(fp);
  fprintf(stdout, "Map load with success\n");
}

#endif
