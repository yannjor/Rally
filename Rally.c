#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Rally.h"


/*Adds a driver named "lastname" and who is representing "team" to buffer of length "size".
  Returns 1 if the operation was successful, 0 otherwise.*/

int add_driver(driver *buffer, char *lastname, char *team) {
    int i;
    for (i = 0; buffer[i].lastname[0] != '\0'; i++) //Find out the size of buffer
    
    buffer = realloc(buffer, (i + 2) * sizeof(driver));
    if (!buffer) return 0;  //if memory allocation failed
    strcpy(buffer[i].lastname, lastname);
    strcpy(buffer[i].team, team);
    buffer[i].time = 0;  //Initially the total time is 0
    buffer[i+1].lastname[0] = '\0';
    return 1;
}


void update_time(driver *buffer, char *lastname, int time) {
    int flag = 0;
    for (int i = 0; buffer[i].lastname[0] != '\0'; i++) {
        if (!strcmp(buffer[i].lastname, lastname)) {
            buffer[i].time += time;
            flag = 1;
        }
    }
    if (!flag) printf("ERROR: Did not find driver: '%s'\n", lastname);
}

//Helper function for print_results:

int compare (const void* p1, const void* p2) {
    driver a = *(driver*)p1;
    driver b  = *(driver*)p2;
    if (a.time < b.time) return -1;
    else if (a.time > b.time) return 1;
    else return 0;
}


void print_results(driver *buffer) {
    //First sort the buffer
    int j;
    for (j = 0; buffer[j].lastname[0] != '\0'; j++);    
    qsort(buffer, j, sizeof(driver), compare);

    //Then print
    printf("Current results of the race:\n\n");
    for (int i = 0; buffer[i].lastname[0] != '\0'; i++) {
        printf("%s - %s - %dh %dmin %ds\n", buffer[i].lastname, buffer[i].team, buffer[i].time / 3600,
        (buffer[i].time % 3600) / 60, (buffer[i].time % 3600) % 60);
    }
}


int write_results(driver *buffer, const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) return 0;

    for (int i = 0; buffer[i].lastname[0] != '\0'; i++) {
        fprintf(fp, "%s - %s - %dh %dmin %ds\n", buffer[i].lastname, buffer[i].team, buffer[i].time / 3600,
        (buffer[i].time % 3600) / 60, (buffer[i].time % 3600) % 60);
    }
    fclose(fp);
    return 1;
}


//Doesn't currently work...

int load_results(driver *buffer, const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return 0;

    buffer = realloc(buffer, sizeof(driver));
    char row[100];
    int i = 0;

    while(fgets(row, 100, fp)) {
        buffer = realloc(buffer, sizeof(driver) * (i + 2));
        int h, min, sec;
        sscanf(row, "%s %s %d %d %d", buffer[i].lastname, buffer[i].team, &h, &min, &sec);
        buffer[i].time = (h * 3600) + (min * 60) + sec;
        i++;
    }
    buffer = realloc(buffer, sizeof(driver) * (i + 2));
    buffer[i+1].lastname[0] = '\0';
    fclose(fp);
    return 1;
}

void exit_program(driver *buffer) {
    printf("Exiting program...\n");
    free(buffer);
    exit(0);
}


int main() {
    
    driver *buffer = malloc(sizeof(driver));
    buffer->lastname[0] = '\0';

    char row[256];
    char command;
    char lastname[50];
    char team[50];
    char filename[50];


    while(1) {

        fgets(row, 256, stdin);

        sscanf(row, "%c", &command);

        if (command == 'A') {
            sscanf(row, "%*c %s %s", lastname, team);
            add_driver(buffer, lastname, team);
        } else if (command == 'U') {
            int h, min, sec;
            sscanf(row, "%*c %s %d %d %d", lastname, &h, &min, &sec);
            int time = (h * 3600) + (min * 60) + sec;
            update_time(buffer, lastname, time);
        } else if (command == 'L') {
            print_results(buffer);
        } else if (command == 'W') {
            sscanf(row, "%*c %s", filename);
            write_results(buffer, filename);
        } else if (command == 'O') {
            //sscanf(row, "%*c, %s", filename);
        } else if (command == 'Q') {
            break;
        } else {
            printf("Unknown command");
        }
    }




    /*add_driver(buffer, "Kankkunen", "Renault");
    update_time(buffer, "Kankkunen", 4325);
    add_driver(buffer, "Räikkönen", "Peugeot");
    update_time(buffer, "Räikkönen", 3333);
    add_driver(buffer, "Svakar Teknolog", "Lada");
    update_time(buffer, "Svakar Teknolog", 333333);
    print_results(buffer);
    write_results(buffer, "results.txt");*/
    return 1;
}