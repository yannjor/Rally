#include "Rally.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Driver *add_driver(Driver *arr, int *size, const char *p_lastname,
                   const char *p_team) {
  // First we check if the driver is not already in the race
  int n = *size;
  for (int i = 0; i < n; i++) {
    if (!strcmp(arr[i].lastname, p_lastname)) {
      printf("Driver '%s' is already in the race\n", p_lastname);
      return arr;
    }
  }

  Driver *ret_arr = realloc(arr, (n + 1) * sizeof(Driver));

  if (!ret_arr) {
    printf("Memory allocation failed when adding driver\n");
    return arr;
  }

  // First we allocate memory for the strings
  ret_arr[n].lastname = malloc(strlen(p_lastname) + 1);
  ret_arr[n].team = malloc(strlen(p_team) + 1);

  // Then copy them
  strcpy(ret_arr[n].lastname, p_lastname);
  strcpy(ret_arr[n].team, p_team);
  ret_arr[n].time = 0;  // Initially the total time is 0
  (*size)++;
  printf("Added driver '%s' to the race\n", p_lastname);
  return ret_arr;
}

void update_time(Driver *arr, int size, char *lastname, int time) {
  int flag = 0;
  for (int i = 0; i < size; i++) {
    if (!strcmp(arr[i].lastname, lastname)) {
      arr[i].time += time;
      flag = 1;
    }
  }
  if (!flag)
    printf("Did not find driver '%s'\n", lastname);
  else
    printf("Updated time for driver '%s'\n", lastname);
}

// Helper function for sorting the array with qsort in the following functions

int compare(const void *p1, const void *p2) {
  Driver a = *(Driver *)p1;
  Driver b = *(Driver *)p2;
  if (a.time < b.time)
    return -1;
  else if (a.time > b.time)
    return 1;
  else
    return 0;
}

void print_results(Driver *arr, int size) {
  if (size == 0)
    printf("There are no drivers in the race\n");
  else {
    // First sort the buffer
    qsort(arr, size, sizeof(Driver), compare);
    // Then print
    printf("\n");
    for (int i = 0; i < size; i++) {
      // We need to convert the time in seconds to hours, minutes and seconds.
      printf("%s %s %dh %dmin %ds\n", arr[i].lastname, arr[i].team,
             arr[i].time / 3600, (arr[i].time % 3600) / 60,
             (arr[i].time % 3600) % 60);
    }
    printf("\nPrinted results successfully\n");
  }
}

void write_results(Driver *arr, int size, const char *filename) {
  FILE *fp = fopen(filename, "w");
  if (!fp) {
    printf("Failed to write results");
    return;
  }

  qsort(arr, size, sizeof(Driver), compare);
  for (int i = 0; i < size; i++) {
    fprintf(fp, "%s %s %dh %dmin %ds\n", arr[i].lastname, arr[i].team,
            arr[i].time / 3600, (arr[i].time % 3600) / 60,
            (arr[i].time % 3600) % 60);
  }
  fclose(fp);
  printf("Wrote current results into file '%s' successfully\n", filename);
}

Driver *load_results(Driver *arr, int *size, const char *filename) {
  FILE *fp = fopen(filename, "r");
  if (!fp) {
    printf("Failed to open file '%s'\n", filename);
    return arr;
  }
  Driver *ret_arr = realloc(arr, sizeof(Driver));

  if (!ret_arr) {
    printf("Memory allocation failed when loading results");
    return arr;
  }

  char line[100];  // Used for storing the contents of the current line of the
                   // file to analyze it with sscanf
  int i = 0;
  char lastname[30];
  char team[30];

  while (fgets(line, 100, fp)) {
    ret_arr = realloc(ret_arr, sizeof(Driver) * (i + 1));
    int h, min, sec;
    sscanf(line, "%s %s %dh %dmin %ds\n", lastname, team, &h, &min, &sec);
    ret_arr[i].lastname = malloc(strlen(lastname) + 1);
    ret_arr[i].team = malloc(strlen(team) + 1);
    strcpy(ret_arr[i].lastname, lastname);
    strcpy(ret_arr[i].team, team);
    ret_arr[i].time = (h * 3600) + (min * 60) + sec;
    i++;
  }
  *size = i;  // Update the size so the main function knows
  fclose(fp);
  printf("Succesfully loaded results from file '%s'\n", filename);
  return ret_arr;
}

void quit_program(Driver *arr, int size) {
  printf("Exiting program...\n");
  for (int i = 0; i < size;
       i++) {  // Go through all drivers and free their names and teams
    free(arr[i].lastname);
    free(arr[i].team);
  }
  free(arr);
}

int main() {
  // Welcome screen

  printf(
      "--- Welcome to the Rally-Results Program! ---\n\n"
      "1. Use function: 'A lastname team' to add a new driver to the race\n"
      "2. Use function: 'U lastname hours minutes seconds' to update the time "
      "of a driver.\n"
      "3. Use function: 'L' to print the current results of the rally.\n"
      "4. Use function: 'W filename' to save the rally results to a file.\n"
      "5. Use function: 'O filename' to load previous rally results from a "
      "file.\n"
      "6. Use function: 'Q' to quit.\n\n");

  Driver *driver_arr =
      malloc(sizeof(Driver));  // Used for storing all the drivers in the race

  int len = 0;         // length of the driver array
  char row[256];       // Used for storing the user's input
  char command;        // Used for storing the current command
  char name[30];       // Used for storing the current driver's lastname
  char team_name[30];  // Used for storing the current driver's teamname
  char file_name[30];  // Used for storing the file name for loading and
                       // printing to files

  while (1) {
    fgets(row, 255, stdin);
    // First check the command
    sscanf(row, "%c", &command);

    if (command == 'A') {
      sscanf(row, "%*c %s %s", name, team_name);
      driver_arr = add_driver(driver_arr, &len, name, team_name);
    } else if (command == 'U') {
      int h, min, sec;
      sscanf(row, "%*c %s %d %d %d", name, &h, &min, &sec);
      int time = (h * 3600) + (min * 60) + sec;
      update_time(driver_arr, len, name, time);
    } else if (command == 'L') {
      print_results(driver_arr, len);
    } else if (command == 'W') {
      sscanf(row, "%*c %s", file_name);
      write_results(driver_arr, len, file_name);
    } else if (command == 'O') {
      sscanf(row, "%*c %s", file_name);
      driver_arr = load_results(driver_arr, &len, file_name);
    } else if (command == 'Q') {
      quit_program(driver_arr, len);
      break;
    } else {
      printf("Unknown command: '%c'\n", command);
    }
  }
  return 1;
}