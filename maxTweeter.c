#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define MAX_SIZE 20000
#define name_size 100
#define MAX_LINE_LENGTH 1024

int expected_columns = 0;
int current_heap_size = 0;
int line_count = 0;
bool name_quoted = false;

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

    return comma_count == expected_columns;
}

int getNameColumn(char* line){
    char* quote_ptr = strstr(line, "\"name\"");
    char* pos_ptr = strstr(line, "name");

    if (quote_ptr + 1 == pos_ptr){
        pos_ptr = strstr(pos_ptr + 1, "name");
    }

    if ((pos_ptr == NULL && quote_ptr == NULL) || (quote_ptr && pos_ptr)){
        error();
    }

    char* ptr = NULL;

    if (quote_ptr){
        if (strstr(quote_ptr + 2, "name")){
            error();
        }

        name_quoted = true;
        ptr = quote_ptr;
    } else if (pos_ptr) {
        if (strstr(pos_ptr + 1, "name")){
            error();
        }

        ptr = pos_ptr;
    } else {
        error();
    }

    int comma_count = 0;
    for (int i = 0; i < strlen(ptr); i++){
        if (ptr[i] == ','){
            comma_count++;
        }
    }

    return expected_columns - comma_count;
}

char* trim(char* name){
    int quote_count = 0;

    if (strlen(name) > 1){
        quote_count = name[0] == '\"'? 1 : 0;
        quote_count = name[strlen(name) - 1] == '\"'? quote_count + 1 : quote_count;

        switch (quote_count) {
            case 0:
              if (name_quoted){
                  error();
              }

              return name;
            case 1:
              error();
              break;
            case 2:
              if (!name_quoted){
                  error();
              }

              name++;
              char* temp = malloc((strlen(name) - 1) * 4);

              int index = 0;
              while (name[index + 1] != '\0'){
                  temp[index] = name[index];
                  index++;
              }

              return temp;
              break;
            default:
              return name;
        }

    }

    return name;

}

char* getfield(char* line, int num)
{
    int comma_count = 0;
    char* name = malloc(MAX_LINE_LENGTH * 4);

    for (int i = 0; i < strlen(line); i++){
        if (comma_count == num){
            int index = 0;
            while (line[i + index] != ','){
                name[index] = line[i + index];
                index++;
            }

            return trim(name);
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

int getNumColumns(char* line)
{
    int comma_count = 0;
    for (int i = 0; i < strlen(line); i++){
        if (line[i] == ','){
            comma_count++;
        }
    }

    return comma_count;
}

void mapInsert(char* name)
{
    long hashcode = hashCode(name);

    if (hashmap[hashcode % MAX_SIZE].name == NULL)
    {
        hashmap[hashcode % MAX_SIZE].name = malloc(strlen(name) * 4);
        strcpy(hashmap[hashcode % MAX_SIZE].name, name);
        hashmap[hashcode % MAX_SIZE].count = 1;
    }
    else
    {
        if (strcmp(hashmap[hashcode % MAX_SIZE].name, name) == 0)
        {
            hashmap[hashcode % MAX_SIZE].count += 1;
        }
        else
        {
            long newIndex = 0;
            while (hashmap[newIndex].name != NULL)
            {
                newIndex++;
            }

            hashmap[newIndex].name = malloc(strlen(name) * 4);
            strcpy(hashmap[newIndex].name, name);
            hashmap[newIndex].count = 1;
        }
    }
}

void outputTopN(int n)
{
    qsort((void*)hashmap, sizeof(hashmap) / sizeof(hashmap[0]), sizeof(hashmap[0]), comparator);
    for(int i = 0; i < n; i++)
    {
        if (hashmap[i].name != NULL){
          printf("%s: %d\n", hashmap[i].name, hashmap[i].count);
        }
    }
}

int main(int argc, char* argv[])
{
    if (argc != 2){
        error();
    }

    FILE* stream = fopen(argv[1], "r");


    if(stream == NULL)
    {
        error();
    }

    char line[2 * MAX_LINE_LENGTH];
    char temp[2 * MAX_LINE_LENGTH];
    fgets(line, 2 * MAX_LINE_LENGTH, stream);
    if (strlen(line) > MAX_LINE_LENGTH) {
        error();
    }
    strcpy(temp, line);

    expected_columns = getNumColumns(temp);

    if (!validColumns(temp)){
        error();
    }

    int nameColumn = getNameColumn(temp);

    while (fgets(line, 2 * MAX_LINE_LENGTH, stream))
    {
        if (strlen(line) > MAX_LINE_LENGTH) {
            error();
        }

        if (line_count == MAX_SIZE){
            error();
        }

        strcpy(temp, line);

        char* name = getfield(temp, nameColumn);
        mapInsert(name);

        line_count++;

    }
    outputTopN(10);
}
