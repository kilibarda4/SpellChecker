#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "spell.h"

/*  Suggestions
- When you work with 2D arrays, be careful. Either manage the memory yourself, or
work with local 2D arrays. Note C99 allows parameters as array sizes as long as 
they are declared before the array in the parameter list. See: 
https://www.geeksforgeeks.org/pass-2d-array-parameter-c/

Worst case time complexity to compute the edit distance from T test words
 to D dictionary words where all words have length MAX_LEN:
Student answer:  Theta(T * D * MAX_LEN^2)

*/


/*
Parameters:
  -string filename - used to read in the dictionary/test file into an array of strings, contains file extension
Return:
  -a pointer to a dynamically allocated array of strings
TC: O(file_length * line_length) //the time complexity depends on the length of file and length of each line
SC: O(file_length * line_length) //for each word we are allocating a pointer and enough memory to store said word
*/
char** file_read(char* filename, int* size)
{
  int i = 0;
  char line[102];
  char** array;

  FILE* fp = fopen(filename, "r");
  if (fp == NULL)
  {
    printf("Unable to open file %s\n", filename);
    return NULL;
  }
  fscanf(fp, "%d\n", size);
  array = malloc(*size * sizeof(char*));
  if(!array)
  {
    printf("Error allocating memory\n");
    return NULL;
  }

  while(fgets(line, 102, fp))
  {
    //search for a newline symbol in each string and replace it with a null character
    char *eol = strchr(line, '\n');
    if(eol)
    {
      *eol = '\0';
    }
    array[i] = malloc( (strlen(line) + 1) * sizeof(char));
    if( !array[i] )
    {
      printf("Error allocating memory for string\n");
      for(int j = 0; j < i; j++)
      {
        free(array[j]);
      }
      free(array);
      return NULL;
    }
    strcpy(array[i], line);
    i++;
  }
  fclose(fp);
  if(filename[0] == 'd')
  {
    printf("\nLoaded dictionary %s has size: %d\n\n", filename, *size);
  }
  return array;
}

/*
Parameters:
  -integers a, b & c
Return:
  -the minimum value of the three integers
TC: O(1)
SC: O(1)
*/
int min(int a, int b, int c)
{
  if (a <= b && a <= c)
  {
    return a;
  }
  else if (b <= a && b <= c)
  {
    return b;
  }
  return c;
}


/*
Utility function to print lines of dashes and \n
//TC: O(1)
//SC: O(1)
*/
void insert_break(int x)
{
  x = x*4 + 8;
  printf("\n");
  for(int i = 0; i < x; i++)
  {
    printf("-");
  }
  printf("\n");
}

/*
Parameters:
  -first and second strings
  -the length of each
  -the array containing the table to print
Return:
  -Does not return anything, prints the 2D array as a formatted table that shows
  -steps in calculating the edit distance
TC: O(1)
SC: O(1)
*/
void distance_table(int first_len, int second_len, int c[first_len+1][second_len+1], char *first_string, char *second_string)
{
  //first row
  printf("   |   |");
  for(int i = 0; i < second_len; i++)
  {
    printf("%3c|", second_string[i]);
  }
  //second row
  insert_break(second_len);
  printf("   |  0|");
  for(int j = 1; j <= second_len; j++)
  {
    printf("%3d|", c[0][j]);
  }
  //the rest of the table
  insert_break(second_len);
  for(int k = 1; k <= first_len; k++)
  {
    printf("%3c|%3d|", first_string[k-1], c[k][0]);
    for(int l = 1; l <= second_len; l++)
    {
      printf("%3d|", c[k][l]);
    }
    insert_break(second_len);
  }
}


/*
Parameters:
  - first_string - pointer to the first string (displayed vertical in the table)
  - second_string - pointer to the second string (displayed horizontal in the table)
  - print_table - If 1, the table with the calculations for the edit distance will be printed.
                  If 0 (or any value other than 1)it will not print the table
Return:  the value of the edit distance
TC: O(N*p) where N = first_len, p = second_len
SC: O(N*p) since we're allocating an N*p array to hold the distances;
*/
int edit_distance(char * first_string, char * second_string, int print_table)
{
  int first_len = strlen(first_string);
  int second_len = strlen(second_string);
  int c[first_len + 1][second_len + 1]; //array to hold the distance table

  //nested for loop to find minimum distance between strings
  //TC : O(second_len*first_len) since min is O(1)
  //SC: O(1) no additional space is used by the loop
  for(int i = 0; i <= first_len; i++)
  {
    for(int j = 0; j <= second_len; j++)
    {
      if(i == 0) //takes care of dist(0,j) and dist(0,0)
      {
        c[i][j] = j;
      }
      else if(j == 0) //takes care of dist(i,0)
      {
        c[i][j] = i;
      }
      else if(first_string[i-1] == second_string[j-1]) //case of matching letters
      {
        c[i][j] = c[i-1][j-1];
      }
      else
      {
        c[i][j] = 1 + min(c[i-1][j], c[i][j-1], c[i-1][j-1]); //non-matching letters
      }
    }
  }
  //call table printing function if specified by the user
  if(print_table == 1)
  {
    distance_table(first_len, second_len, c, first_string, second_string);
  }
  return c[first_len][second_len];
}

/*
Parameters:
  - testname - string containing the name of the file with the paragraph to spell check, includes file extenssion e.g. "text1.txt" 
  - dictname - name of file with dictionary words. Includes file extenssion, e.g. "dsmall.txt"
Behavior: If any of the files cannot be open displays a message and returns. (Does not exit the program.)
*/
void spell_check(char * testname, char * dictname){
  int *dict_size = malloc(sizeof(int));
  int *list_size = malloc(sizeof(int));
  char** dict = file_read(dictname, dict_size);
  char** test_file = file_read(testname, list_size);
  
  //if either of the pointers fails to load, free all and return
  if(!dict || !test_file)
  {
    printf("There was a problem related to file io or memory allocation\n");
    if(dict)
    {
    for(int f = 0; f < *dict_size; f++)
    {
      free(dict[f]);
    }
    free(dict);
    }
    free(dict_size);

    if(test_file)
    {
    for(int f = 0; f < *list_size; f++)
    {
      free(test_file[f]);
    }
      free(test_file);
    }
    free(list_size);
    return;
  }

  int num = 0, min_distance = 101, count;
  int distances[*dict_size]; //array to store edit distances
  int index[*dict_size]; //array to store the index of closest distance word in dict
  char correction[100]; //to hold the string if user decides to change spelling
  char c;

  for(int i = 0; i < *list_size; i++)
  {
    count = 1;
    printf("\n--->|%s|\n", test_file[i]);

    //O(dict_size * N * p)) //where N and p are lengths of test_file[i] and dict[j]
    for(int j = 0; j < *dict_size; j++)
    {
      distances[j] = edit_distance(test_file[i], dict[j], 0);
      min_distance = (distances[j] < min_distance) ? distances[j] : min_distance;
    }
    printf("-1 - Type correction\n 0 - Ignore\n\n");
    printf("     Minimum distance: %d\n", min_distance);
    printf("     Words that give minimum distance:\n");

    //O(dict_size * N * p) again, which gets added up with the previous one
    for(int k = 0; k < *dict_size; k++)
    {
      if(min_distance == edit_distance(test_file[i], dict[k], 0))
      {
        printf(" %d - %s\n", count, dict[k]);
        index[count] = k;
        count++;
      }
    }
    printf("Enter your choice:");
    scanf("%d", &num);
    if(num == -1)
    {
      printf("Enter new spelling: ");
      scanf("%s%c", correction, &c);
      printf("Corrected word is: %s\n", correction);
    }
    else if(num == 0)
    {
      printf("Ignoring spelling, word is: %s\n", test_file[i]);
    }
    else if(num > 0 && num < count)
    {
      printf("%d\n", num);
      printf("Corrected word is: %s\n", dict[index[num]]);
    }
    else
    {
      printf("Invalid command, keeping original word: %s\n", test_file[i]);
    }
    insert_break(10); //O(1) utility function
  }
  //free all dynamically allocated memory
  for(int f = 0; f < *dict_size; f++)
  {
    free(dict[f]);
  }
  free(dict);
  free(dict_size);
  for(int f = 0; f < *list_size; f++)
  {
    free(test_file[f]);
  }
  free(list_size);
  free(test_file);
}