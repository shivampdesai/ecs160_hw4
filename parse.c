
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

char* getfield(char* line, int num)
{
  int comma_count = 0;
  char* name = malloc(1024 * 4);

  for (int i = 0; i < strlen(line); i++){
    if (comma_count == 8){
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

  return "INVALID";
}

int main(){
  FILE* stream = fopen("cl-tweets-short-clean.csv", "r");

  char line[1024];
  int i = 0;
  while (fgets(line, 1024, stream))
  {
      char temp[1024];
      strcpy(temp, line);

      char* name = getfield(temp, 8);
      printf("%s\n", name);
      i++;
  }
}
