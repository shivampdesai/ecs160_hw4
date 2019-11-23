#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#define MAX_SIZE 20000
#define name_size 100
#define heap_size 10 //need extra index for insertion? or replace smallest element?
#define expected_columns 16

int current_heap_size = 0;

struct tweet {
    int count;
    char* name;
};

struct tweet hashmap[MAX_SIZE];

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

/*
int parent(int i)
{
  return (i - 1) / 2;
}

int left(int i)
{
  return 2 * i + 1;
}

int right(int i)
{
  return 2 * i + 2;
}

void maxheapify(struct tweet *arr, int i)
{
  int l, r, largest;
  struct tweet temp;
  l = left(i);
  r = right(i);
  if(l < current_heap_size && arr[l].count > arr[i].count)
    largest = l;
  else
    largest = i;
  if(r < current_heap_size && arr[r].count > arr[largest].count)
    largest = r;
  //printf("\ni=%d  l=%d  r=%d largest=%d",i,l,r,largest);
  if(largest != i)
  {
    temp = arr[i];
    arr[i] = arr[largest];
    arr[largest] = temp;
    maxheapify(arr, largest);
  }
}

int findMin(struct tweet * heap)
{
    int min_index = 0;
    for(int i = 1; i < current_heap_size; i++)
    {
        if(heap[i].count < heap[min_index].count)
        {
            min_index = i;
        }
    }
    return min_index;
}

void heapInsert(struct tweet * heap, struct tweet new_val)
{
    if(current_heap_size < heap_size - 1)
    {
        current_heap_size++;
        heap[current_heap_size - 1] = new_val;
        maxheapify(heap, current_heap_size - 1);
    }
    else
    {
        int min_index = findMin(heap);
        if(new_val.count <= heap[min_index].count)
        {
            return;
        }
        else
        {
            heap[min_index] = new_val;
            maxheapify(heap, min_index);
        }
    }
}

void buildHeap(struct tweet * heap)
{
    for(int i = 0; i < MAX_SIZE; i++)
    {
        if(hashmap[i].name != NULL) //non empty map value
        {
            heapInsert(heap, hashmap[i]);
        }
    }
}

struct tweet extractMax(struct tweet * heap)
{
    struct tweet max = heap[0];
    heap[0] = heap[current_heap_size--];
    maxheapify(heap, 0);
    return max;
}

void printHeap(struct tweet * heap)
{
    int num_to_print = current_heap_size;
    for(int i = 0; i < num_to_print; i++)
    {
        struct tweet toPrint = extractMax(heap);
        printf("%s: %d\n", toPrint.name, toPrint.count);
    }
}
*/

int comparator(const void* p1, const void* p2)
{
   int first = ((struct tweet *)p1)->count;
   int second = ((struct tweet *)p2)->count;
   return second - first;
}



int main()
{


    FILE* stream = fopen("cl-tweets-short-clean.csv", "r");
    struct tweet * heap = malloc(heap_size * sizeof(struct tweet));

    char line[1024];
    char temp[1024];
    fgets(line, 1024, stream);
    strcpy(temp, line);

    if (!validColumns(temp)){
      printf("Invalid Input File\n");
      return -1;
    }

    int nameColumn = getNameColumn(temp);

    while (fgets(line, 1024, stream))
    {

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

    }

    //iterate();
	qsort((void*)hashmap, sizeof(hashmap) / sizeof(hashmap[0]), sizeof(hashmap[0]), comparator);
    //buildHeap(heap);
    //printHeap(heap);
    for(int i = 0; i < 10; i++)
    {
        printf("%s: %d\n", hashmap[i].name, hashmap[i].count);
    }

}
