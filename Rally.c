#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Rally.h"




Driver *add_driver(Driver *arr, int size, const char *p_lastname, const char *p_team) {
    

    Driver *ret_arr = realloc(arr, (size + 1) * sizeof(Driver));
    if (!ret_arr) printf("Memory allocation failed\n");

    ret_arr[size].lastname = malloc(strlen(p_lastname) + 1);
    ret_arr[size].team = malloc(strlen(p_team) + 1);

    strcpy(ret_arr[size].lastname, p_lastname);
    strcpy(ret_arr[size].team, p_team);
    ret_arr[size].time = 0;  //Initially the total time is 0

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
    if (!flag) printf("Did not find driver '%s'\n", lastname);
    else printf("Updated time for driver '%s'\n", lastname);
}

//Helper function for print_results:

int compare (const void* p1, const void* p2) {
    Driver a = *(Driver*)p1;
    Driver b  = *(Driver*)p2;
    if (a.time < b.time) return -1;
    else if (a.time > b.time) return 1;
    else return 0;
}


void print_results(Driver *arr, int size) {
    
    if (size == 0) printf("There are no drivers in the race\n");
    else {
        //First sort the buffer

        qsort(arr, size, sizeof(Driver), compare);
        //Then print

        for (int i = 0; i < size ; i++) {
            printf("%s - %s - %dh %dmin %ds\n", arr[i].lastname, arr[i].team, arr[i].time / 3600,
            (arr[i].time % 3600) / 60, (arr[i].time % 3600) % 60);
        }
    }
}


int write_results(Driver *arr, int size, const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) return 0;

    for (int i = 0; i < size; i++) {
        fprintf(fp, "%s - %s - %dh %dmin %ds\n", arr[i].lastname, arr[i].team, arr[i].time / 3600,
        (arr[i].time % 3600) / 60, (arr[i].time % 3600) % 60);
    }
    fclose(fp);
    return 1;
}


Driver *load_results(Driver *arr, int *size, const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Failed to open file '%s'\n", filename);
        return arr;
    }
    Driver *ret_arr = realloc(arr, sizeof(Driver));

    char row[100];
    int i = 0;
    char name[30];
    char team[30];


    while(fgets(row, 100, fp)) {
        ret_arr = realloc(ret_arr, sizeof(Driver) * (i + 1));
        int h, min, sec;
        sscanf(row, "%s - %s - %dh %dmin %ds\n", name, team, &h, &min, &sec);
        ret_arr[i].lastname = malloc(strlen(name) + 1);
        ret_arr[i].team = malloc(strlen(team) + 1);
        strcpy(ret_arr[i].lastname, name);
        strcpy(ret_arr[i].team, team);
        ret_arr[i].time = (h * 3600) + (min * 60) + sec;
        (*size)++;
        i++;
    }

    fclose(fp);
    return ret_arr;
}


int main() {
    
    Driver *driver_arr = malloc(sizeof(Driver));
    

    int len = 0;
    char row[256];
    char command;
    char name[30];
    char team_name[30];
    char file_name[30];


    while(1) {

        fgets(row, 255, stdin);
        sscanf(row, "%c", &command);

        if (command == 'A') {
            sscanf(row, "%*c %s %s", name, team_name);
            driver_arr = add_driver(driver_arr, len, name, team_name);
            len++;
        } else if (command == 'U') {
            int h, min, sec;
            sscanf(row, "%*c %s %d %d %d", name, &h, &min, &sec);
            int time = (h * 3600) + (min * 60) + sec;
            update_time(driver_arr, len, name, time);
        } else if (command == 'L') {
            print_results(driver_arr, len);
        } else if (command == 'W') {
            sscanf(row, "%*c %s", file_name);
            if (write_results(driver_arr, len, file_name)) {
                printf("Wrote current results into file '%s' successfully\n", file_name);
            } else printf("Failed to write results");
        } else if (command == 'O') {
            sscanf(row, "%*c %s", file_name);
            driver_arr = load_results(driver_arr, &len, file_name);
        } else if (command == 'Q') {
            printf("Exiting program...\n");
            free(driver_arr);
            break;
        } else {
            printf("Unknown command: '%c'\n", command);
        }
    }
    
    return 1;
}