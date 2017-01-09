#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "map.h"
#include "error.h"
#include "list.h"

#ifdef PADAWAN

#define BUFFER_MAX_SIZE 300

void natural_write(int fd, char str[BUFFER_MAX_SIZE]){
  if(write(fd, str, sizeof(char)*strlen(str)) == -1){
      exit_with_error ("Map save failed: 'write'' function\n");
    }
}

int init_saving_file(char *filename){
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

int save_obj_on_map(int *array, int obj_val, int iterator){
  // si l'objet n'est as encore dans la liste
  if(!isValueInArray(obj_val, array, iterator)) {
    // stockage de l'ID du nouvel objet decouvert
    array[iterator] = obj_val;
    return 1;
  }
  return 0;
}

void print_map_status(int fd, int map_width, int map_height, int map_nb_obj){
  char buffer[BUFFER_MAX_SIZE];
  sprintf(buffer, "%d\n%d\n%d\n", map_width, map_height, map_nb_obj);
  natural_write(fd, buffer);
}

int print_map_matrix(int fd, int map_width, int map_height, int *objects_array){
  char buffer[BUFFER_MAX_SIZE];
  int object_value_tmp;
  int iterator = 0;
  // write de la matrice de la map (bas->haut, gauche->droite)
  natural_write(fd, "**\n");
  for(int i = map_height-1; i >= 0; i--){
    for(int j = 0; j < map_width; j++){
      object_value_tmp = map_get(j,i);
      sprintf(buffer, "%d ", object_value_tmp);
      natural_write(fd, buffer);
      
      // récupération à la volee des différents objets présents sur la map
      if(save_obj_on_map(objects_array, object_value_tmp, iterator)){
        iterator++;
      }
    }
    natural_write(fd, "\n");
  }
  natural_write(fd, "**\n");
  return iterator;
}

void print_saved_obj(int fd, int *array, int iterator){
  char buffer[BUFFER_MAX_SIZE];
  int curr_obj;
  char *obj_name;
  int obj_frames;
  int obj_solid;
  int obj_destruct;
  int obj_collect;
  int obj_gener;
  for(int i = 0; i < iterator; i++){
    curr_obj = array[i];
    obj_name = map_get_name(curr_obj);
    obj_frames = map_get_frames(curr_obj);
    obj_solid = map_get_solidity(curr_obj);
    obj_destruct = map_is_destructible(curr_obj);
    obj_collect = map_is_collectible(curr_obj);
    obj_gener = map_is_generator(curr_obj);
    sprintf(buffer, "%d %d %d %d %d %d\n", curr_obj, obj_frames, obj_solid, obj_destruct, obj_collect, obj_gener);
    //natural_write(fd, obj_name);
    natural_write(fd, buffer);
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

  map_set(2, height-2, 2);
  //map_set(2, height-2, 3);
  //map_set(2, height-2, 4);
  //map_set(2, height-2, 5);

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

void map_save (char *filename)
{
  int width = map_width();
  int height = map_height();
  int nb_obj = map_objects();

  int objects_array[nb_obj];
  int nb_objects_saved;

  int fd = init_saving_file(filename);
  print_map_status(fd, width, height, nb_obj);
  nb_objects_saved = print_map_matrix(fd, width, height, objects_array);
  print_saved_obj(fd, objects_array, nb_objects_saved);
  
  close(fd);
  fprintf(stderr, "Map saved on: %s\n", filename);
}

void map_load (char *filename)
{
  /*// TODO_begin
  char buffer;
  int fd = open(filename, O_WRONLY|O_CREAT, 0666);
  if(fd == -1){
    exit_with_error("Map_load failed when opening file\n");
  }

  printf("")

  for(i = input_length; i >=0 ; i--){
    lseek(fd1,i,SEEK_SET);
    read(fd1, &buffer, sizeof(char));
    write(fd2, &buffer, sizeof(char));
  }


  close(fd);*/
  // TODO_end
  exit_with_error ("Map load is not yet implemented\n");
}

#endif
