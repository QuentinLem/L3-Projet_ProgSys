#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "map.h"
#include "error.h"

#ifdef PADAWAN

#define BUFFER_MAX_SIZE 200

typedef struct Element Element;
struct Element
{
   int value;
   char description[BUFFER_MAX_SIZE];
   Element *next;
};

typedef struct list_elem_map List;
struct List
{
    Element *first;
};

List *list_init(int value, char description[BUFFER_MAX_SIZE])
{
   List *list = malloc(sizeof(struct List));
   Element *element = malloc(sizeof(*element));
   if (list == NULL || element == NULL)
   {
       exit(EXIT_FAILURE);
   }
   element->value = value;
   sprintf(element->description, "%s", description);
   element->next = NULL;
   list->first = element;
   return list;
}


void list_add_elem(List *list, int value, char* description)
{
   // Création du nouvel élémen
   Element *newElem = malloc(sizeof(*newElem));
   if (list == NULL || newElem == NULL)
   {
       exit(EXIT_FAILURE);
   }
   newElem->value = value;
   sprintf(newElem->description, "%s", description);
   // Insertion de l'élément au début de la liste
   newElem->next = list->head;
   list->head = newElem;

}

// Supression de l'element en tete de liste
void list_delete_first_elem(List *list)
{
   if (list == NULL) {
       exit(EXIT_FAILURE);
   }
   if (list->first != NULL)
   {
       Element *toDelete = list->first;
       list->first = list->first->next;
       free(toDelete);
   }
}

// Renvoie la taille de notre liste chaînée
int list_length(List *list)
{
    int length = 0;
    while (list->head->next != NULL)  //tant que la fin de la liste n'est pas atteinte 
     {
       length++;
       list = list->next;             //passer à l'élément suivant 
     }
     return length;
}

// Renvoie 1 si l'élément existe dans la liste, 0 sinon
int list_search_element(List *list, int value){
  List *tmp = list;
  while(tmp->first->next!=NULL){
    if(tmp->first->value == value){
      return 1;
    } else {
      tmp->first = tmp->first->next;
    }
  }
  return 0;
}

void list_toString(List *list) {
   if (list == NULL) {
       exit(EXIT_FAILURE);
   }
   char description[BUFFER_MAX_SIZE];
   Element *current = list->first;
   while (current != NULL) {
       sprintf(description, "%s\n", current->description);
       fprintf(stderr, description);
       current = current->next;
   }
   printf("NULL\n");
}
// */

void map_new (unsigned width, unsigned height)
{
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
  int object_value_tmp;
  List *objects = NULL;
  List *tmp;

  int fd = open(filename, O_WRONLY|O_CREAT, 0666);
  if(fd == -1){
    exit_with_error("Map_save failed when opening file\n");
  }
  
  // récupération variables d'environnement de map
  int width = map_width();
  int height = map_height();
  int nb_obj = map_objects();
  
  // concatenation pour write des variables 
  sprintf(buffer, "%d\n%d\n", width, height);
  write(fd, &buffer, sizeof(char)*strlen(buffer));

  // write de la matrice de la map
  for(int i=0; i<height; i++){
    for(int j=0; j<width; j++){
      object_value_tmp = map_get(j,i);
      sprintf(buffer, "%d ", object_value_tmp);
      write(fd, &buffer, sizeof(char)*strlen(buffer));
      
      // récupération des différents objets présents sur la map
      // si la liste est vide
      if(list_length(objects) == 0) {
        objects = list_init(object_value_tmp, object_str_tmp);
      // si l'objet n'est as encore dans la liste
      } else if(!list_search_element(objects, object_value_tmp)) {
        // stockage des données relatives à l'objet
        sprintf(object_str_tmp, "%d::%d:%d:%d:%d:%d:%d", object_value_tmp, map_get_frames(object_value_tmp), map_get_solidity(object_value_tmp), map_is_collectible(object_value_tmp), map_is_destructible(object_value_tmp), map_is_generator(object_value_tmp));
        list_add_elem(objects, object_value_tmp, object_str_tmp);
      }
    }
    write(fd, "\n", sizeof(char));
  }

  int list_size = list_length(objects);
  if(nb_obj != list_size){
    fprintf(stderr, "note: NB OBJECTS !");
    nb_obj = list_size;
  }
  
  tmp = objects;
  for(int i=0; i<nb_obj; i++){
    fprintf(stderr, "%s", tmp->description);
    tmp->first = tmp->next;
  }

  //*/

  close(fd);
  // TODO_end
  fprintf(stderr, "Sorry: Map save is not yet implemented\n");
}

void map_load (char *filename)
{
  // TODO_begin
  char buffer;
  int fd = open(filename, O_WRONLY|O_CREAT, 0666);
  if(fd == -1){
    exit_with_error("Map_load failed when opening file\n");
  }

  /*printf("")

  for(i = input_length; i >=0 ; i--){
    lseek(fd1,i,SEEK_SET);
    read(fd1, &buffer, sizeof(char));
    write(fd2, &buffer, sizeof(char));
  }*/


  close(fd);
  // TODO_end
  exit_with_error ("Map load is not yet implemented\n");
}

#endif
