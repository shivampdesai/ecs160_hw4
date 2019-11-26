#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#define MAX_SIZE 20000
#define name_size 100
#define expected_columns 16

int current_heap_size = 0;
int line_count = 0;

struct tweet {
    int count;
    char* name;
};

struct tweet hashmap[MAX_SIZE];

void error()
{
    printf("Invalid Input File\n");
    exit(1);
}

bool validColumns(char* line){
  int comma_count = 0;
  for (int i = 0; i < strlen(line); i++){
    if (line[i] == ','){
      comma_count++;
    }
  }

  //expect 16 columns -> 15 commas
  return comma_count + 1 == expected_columns;
}

int getNameColumn(char* line){
  char* pos_ptr = strstr(line, "name");

  if (pos_ptr == NULL){
    error();
  }

  int comma_count = 0;
  for (int i = 0; i < strlen(pos_ptr); i++){
    if (pos_ptr[i] == ','){
      comma_count++;
    }
  }

  return expected_columns - 1 - comma_count;
}

char* getfield(char* line, int num)
{
  int comma_count = 0;
  char* name = malloc(1024 * 4);

  for (int i = 0; i < strlen(line); i++){
    if (comma_count == num){
      int index = 0;
      while (line[i + index] != ','){
        name[index] = line[i + index];
        index++;
      }

      return name;
    }

    if (line[i] == ','){
      comma_count++;
    }
  }

  return "NULL";
}


long hashCode(const char *str) {
    long hash = 0;

    for (int i = 0; i < strlen(str); i++) {
        hash = 31 * hash + str[i];

    }

    return labs(hash);
}

void iterate(){
    for (int i = 0; i < MAX_SIZE; i++){
      if (hashmap[i].name != NULL){
        printf("%s: %d\n", hashmap[i].name, hashmap[i].count);
      }
    }
}

int comparator(const void* p1, const void* p2)
{
   int first = ((struct tweet *)p1)->count;
   int second = ((struct tweet *)p2)->count;
   return second - first;
}

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        error();
    }
    FILE* stream = fopen(argv[1], "r");
    if(stream == NULL)
    {
        error();
    }

    char line[1024];
    char temp[1024];
    fgets(line, 1024, stream);
    strcpy(temp, line);

    if (!validColumns(temp)){
      error();
    }

    int nameColumn = getNameColumn(temp);

    while (fgets(line, 1024, stream))
    {

        if (line_count == MAX_SIZE){
          error();
        }

        strcpy(temp, line);

        char* name = getfield(temp, nameColumn);

        long hashcode = hashCode(name);

        if (hashmap[hashcode % MAX_SIZE].name == NULL){
            hashmap[hashcode % MAX_SIZE].name = malloc(strlen(name) * 4);
            strcpy(hashmap[hashcode % MAX_SIZE].name, name);
            hashmap[hashcode % MAX_SIZE].count = 1;
        } else {
            if (strcmp(hashmap[hashcode % MAX_SIZE].name, name) == 0){
              hashmap[hashcode % MAX_SIZE].count += 1;
            } else {
              long newIndex = 0;
              while (hashmap[newIndex].name != NULL){
                newIndex++;
              }

              hashmap[newIndex].name = malloc(strlen(name) * 4);
              strcpy(hashmap[newIndex].name, name);
              hashmap[newIndex].count = 1;
            }
        }

        line_count++;

    }

	qsort((void*)hashmap, sizeof(hashmap) / sizeof(hashmap[0]), sizeof(hashmap[0]), comparator);
    for(int i = 0; i < 10; i++)
    {
        printf("%s: %d\n", hashmap[i].name, hashmap[i].count);
    }
}
