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
bool name_last_column = false;

/*
Struct to store each name and count
*/
struct tweet {
    int count;
    char* name;
};

/*
Hashmap to keep track of count
*/
struct tweet hashmap[MAX_SIZE];

/*
Generic function to print out error message and exit
*/
void error()
{
    printf("Invalid Input File\n");
    exit(1);
}

/*
Check whether there is a valid number of columns in the line
*/
bool validColumns(char* line){
    int comma_count = 0;
    for (int i = 0; i < strlen(line); i++){
        if (line[i] == ','){
            comma_count++;
        }
    }

    return comma_count == expected_columns;
}

/*
Parses the header line to find the column that contains the name
*/
int getNameColumn(char* line){
    char* quote_ptr = strstr(line, "\"name\"");
    char* pos_ptr = strstr(line, "name");

    if (quote_ptr + 1 == pos_ptr){
        pos_ptr = strstr(pos_ptr + 1, "name");
    }

    //if name or "name" is not found, or both are found
    if ((pos_ptr == NULL && quote_ptr == NULL) || (quote_ptr && pos_ptr)){
        error();
    }

    char* ptr = NULL;

    if (quote_ptr){
	//if name is found again
        if (strstr(quote_ptr + 2, "name")){
            error();
        }

        name_quoted = true;
        ptr = quote_ptr;
    } else if (pos_ptr) {
	//if name is found again
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


/*
If the name has 2 outer quotes, remove and return the new name. If there is 
only 1 return an error.
*/
char* trim(char* name){
    int quote_count = 0;

    if (strlen(name) > 1){

      //remove new line character if name is in last column
      if (name_last_column){
        name[strlen(name) - 1] = '\0';
      }

      quote_count = name[0] == '\"'? 1 : 0;
      quote_count = name[strlen(name) - 1] == '\"'? quote_count + 1 : quote_count;

      switch (quote_count) {
          case 0:
            //if name is expected to be quoted but is not
            if (name_quoted){
                error();
            }

            return name;
          case 1:
  	    //mismatch in outer quotes
            error();
            break;
          case 2:
	    //if name is not supposed to be quoted but is	     
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

/*
For a given line, return the value in the column that passed in
*/
char* getfield(char* line, int num)
{
    int comma_count = 0;
    char* name = malloc(MAX_LINE_LENGTH * 4);

    for (int i = 0; i < strlen(line); i++){
        if (comma_count == num){
            int index = 0;
            while (i + index < strlen(line) && line[i + index] != ','){
                name[index] = line[i + index];
                index++;
            }

            return trim(name);
        }

        if (line[i] == ','){
            comma_count++;
        }
    }

    free(name);
    return "NULL";
}

/*
Function to calculate hashCode of a name. Used labs to handle
negative hashcodes.
*/
long hashCode(const char *str) {
    long hash = 0;

    if (str == NULL){
	     return hash;
    }

    for (int i = 0; i < strlen(str); i++) {
        hash = 31 * hash + str[i];
    }

    return labs(hash);
}

/*
Print out all elements in the hashmap
*/
void iterate(){
    for (int i = 0; i < MAX_SIZE; i++){
        if (hashmap[i].name != NULL){
            printf("%s: %d\n", hashmap[i].name, hashmap[i].count);
        }
    }
}

/*
Comparator function to sort tweets in the hashmap
*/
int comparator(const void* p1, const void* p2)
{
    int first = ((struct tweet *)p1)->count;
    int second = ((struct tweet *)p2)->count;
    return second - first;
}

/*
Return the expected number of columns from the header line
*/
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

/*
Inserts/increments the count of the given name in the hashmap 
*/
void mapInsert(char* name)
{
    //if the name passed in is NULL, assign it to 'empty' tweeter
    if (name == NULL){
	name = malloc(24);
	strcpy(name, "empty");
    }

    long hashcode = hashCode(name);

    if (hashmap[hashcode % MAX_SIZE].name == NULL)
    {
        hashmap[hashcode % MAX_SIZE].name = malloc((strlen(name) + 1) * 4);
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

            hashmap[newIndex].name = malloc((strlen(name) + 1) * 4);
            strcpy(hashmap[newIndex].name, name);
            hashmap[newIndex].count = 1;
        }
    }
}

/*
Outputs the top N tweets in the hashmap
*/
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
    //if no input file is provided
    if (argc != 2){
        error();
    }

    FILE* stream = fopen(argv[1], "r");

    //if an empty file is passed in
    if (stream == NULL)
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
    
    if (expected_columns == nameColumn){
      name_last_column = true;
    }

    /*
    Iterate over the lines in the input file -> retrive the name and 
    insert into the hashmap.
    */
    while (fgets(line, 2 * MAX_LINE_LENGTH, stream))
    {
	//if size of line is greater than the max (1024)
        if (strlen(line) > MAX_LINE_LENGTH) {
            error();
        }

	//if the number of lines read in is greater than max (20000)
        if (line_count == MAX_SIZE){
            error();
        }

        strcpy(temp, line);

        char* name = getfield(temp, nameColumn);

	mapInsert(name);

        line_count++;
    }

    outputTopN(10);

    return 0;
}
