#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "map.h"
#include "error.h"
#include "list.h"

#ifdef PADAWAN

#define BUFFER_MAX_SIZE 250

int isValueInArray(int val, int *array, int array_size){
  for(int i = 0; i < array_size; i++){
    if(array[i] == val){
      return 1;
    }
  }
  return 0;
}

int save_obj(int *array, int obj_val, int iterator){
  char* buffer[BUFFER_MAX_SIZE];
  // si l'objet n'est as encore dans la liste
  if(!isValueInArray(obj_val, array, iterator)) {
    // stockage des données relatives à l'objet
    array[iterator] = obj_val;
    sprintf(buffer, "\n%d::%d:%d:%d:%d:%d:%d\n", obj_val, map_get_frames(obj_val), map_get_solidity(obj_val), map_is_collectible(obj_val), map_is_destructible(obj_val), map_is_generator(obj_val));
    return 1;
  }
  return 0;
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
  // TODO_begin
  char buffer[BUFFER_MAX_SIZE];
  char object_str_tmp[BUFFER_MAX_SIZE];
  
  int width = map_width();
  int height = map_height();
  int nb_obj = map_objects();

  int objects_array[nb_obj];
  int object_value_tmp;
  int array_iterator = 0;

  int fd = open(filename, O_WRONLY|O_CREAT, 0666);
  if(fd == -1){
    exit_with_error("Map_save failed when opening file\n");
  }
  
  // concatenation pour write des variables 
  sprintf(buffer, "%d\n%d\n", width, height);
  if(write(fd, &buffer, sizeof(char)*strlen(buffer)) == -1){
    exit_with_error ("Map save failed: write fct\n");
  }

  // write de la matrice de la map
  for(int i=0; i<height; i++){
    for(int j=0; j<width; j++){
      object_value_tmp = map_get(j,i);
      sprintf(buffer, "%d ", object_value_tmp);
      if(write(fd, &buffer, sizeof(char)*strlen(buffer)) == -1){
        exit_with_error ("Map save failed: write fct\n");
      }
      
      // récupération des différents objets présents sur la map
      if(save_obj(objects_array, object_value_tmp, array_iterator)){
        array_iterator++;
      }
    }
    if(write(fd, "\n", sizeof(char)) == -1){
      exit_with_error ("Map save failed: write fct\n");
    }
  }

  /*int list_size = list_length(objects);
  if(nb_obj != list_size){
    fprintf(stderr, "note: NB OBJECTS !");
    nb_obj = list_size;
  }
  
  tmp = objects;
  for(int i=0; i<nb_obj; i++){
    fprintf(stderr, "%s", tmp->description);
    tmp->first = tmp->next;
  }*/

  //

  close(fd);
  // TODO_end*/
  fprintf(stderr, "Sorry : Map save is not totally implemented\n");
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
