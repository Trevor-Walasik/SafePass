/* Copyright (c) 2025 Trevor Walasik
 * All rights reserved
 *
 * Welcome to SafePass, a password management program. Currently SafePass is ran in a terminal and
 * uses keyboard input to add and recall users login credentials. Formaly added passwords are accesed
 * by entering keys associated with the service, these keys are automatically chosen based on the order
 * services are added.
 * 
 * The way SafePass works is by writing to a text file the credentials a user wants to remember. This text 
 * file is then read into memory each time the program is run, storing the credentials in the heap. The 
 * credentials are stored in Services structure defined in SafePassStructDefs.h
 */



# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include "SafePass.h"
# include "SafePassStructDefs.h"

# define clear_terminal() printf("\033[H\033[2J")
# define print_line_of_dashes() printf("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n")
# define LINELENGTH 61

Services s = {NULL, 0};

int page = 0;
int main(void) {
    /* Create pkenc.txt file if not yet created */
    FILE *f = fopen("pkenc.txt", "r");
    if (f == NULL) {
            f = fopen("pkenc.txt", "w");
            fputs("Do Not Edit Text File, For SafePass Use Only", f);
    } else {
        /* Populate array of map pointers ordered by their key values */
        char read_buffer[LINELENGTH];
        fgets(read_buffer, LINELENGTH, f);
        printf("%s", read_buffer);
        
        while (fgets(read_buffer, LINELENGTH, f) != NULL) {
            Map *new_map = malloc(sizeof(*new_map));
            new_map->key = select_key();

            read_buffer[strcspn(read_buffer, "\n")] = 0;
            printf("%s", read_buffer);
            new_map->service_name = malloc(strlen(read_buffer) + 1);
            strcpy(new_map->service_name, read_buffer);


            fgets(read_buffer, LINELENGTH, f);
            read_buffer[strcspn(read_buffer, "\n")] = 0;
            printf("%s", read_buffer);
            new_map->username = malloc(strlen(read_buffer) + 1);
            strcpy(new_map->username, read_buffer);

            fgets(read_buffer, LINELENGTH, f);
            read_buffer[strcspn(read_buffer, "\n")] = 0;
            printf("%s", read_buffer);
            new_map->password = malloc(strlen(read_buffer) + 1);
            strcpy(new_map->password, read_buffer);

            s.service_count++;
            s.map_array = realloc(s.map_array, s.service_count*sizeof(Map *));
            s.map_array[s.service_count - 1] = new_map;
        } 
               
    }

    fclose(f);

    /* Begin Programs Main Loop */
    main_loop(); 
    return 0;
}

int main_loop(void) {

    clear_terminal();
    /* The main loop will display the First five services in the file then information about what the user can input */

    print_user_interface();

    execute_input();
    
    main_loop();

    return 1;
}   

int print_services(void) {
    int starting_index = page*8;
    int i;
    for (i = starting_index; i < starting_index + 8 && i < s.service_count; i++) {
        seventy_pad(s.map_array[i]->key, s.map_array[i]->service_name);
        print_line_of_dashes();
    }
    return 0;
}

int print_user_interface(void) {
    print_line_of_dashes();
    printf("|   Enter Services Key: (1, 2, 3, ... , x, y, z) For Password     |\n");
    print_line_of_dashes();
    print_services();
    printf("| < Previous Page |  Enter '+' to Add New Service   | > Next Page |\n");
    print_line_of_dashes();
    return 0;
}

int seventy_pad(char key, char *main_string) {
    int pad = LINELENGTH - strlen(main_string);
    int i;
    printf("| %c |", key);
    if (strlen(main_string) % 2 == 0) {
        for (i = 0; i < pad/2; i++) {
            printf(" ");
        }
        printf("%s", main_string);
        for (i = 0; i < pad/2 + 1; i++) {
            printf(" ");
        }
    } else {
        for (i = 0; i < pad/2; i++) {
            printf(" ");
        }
        printf("%s", main_string);
        for (i = 0; i < pad/2; i++) {
            printf(" ");
        }
    }
    printf("|\n");
    return 0;
}

int execute_input(void) {
    char input_s[LINELENGTH];
    scanf("%s", &input_s);
    char input = input_s[0];
    if (input == '+') {
        add_credentials();
    } else if (input == '>') {
        next_page();
    } else if (input == '<') {
        prev_page();
    } else {
        run_service(input);
    }
    return 0;
}

int add_credentials(void) {
    char service_name[LINELENGTH];
    char username[LINELENGTH];
    char password[LINELENGTH];
    clear_terminal();

    /* flush the stdin */
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);

    printf("Enter the name of the new service being added: \n");

    fgets(service_name, LINELENGTH, stdin);
    service_name[strcspn(service_name, "\n")] = 0;

    printf("Enter the user name for %s:\n", service_name);

    fgets(username, LINELENGTH, stdin);
    username[strcspn(username, "\n")] = 0;

    printf("Enter the password for %s:\n", service_name);

    fgets(password, LINELENGTH, stdin);
    password[strcspn(password, "\n")] = 0;

    create_map_and_add_to_array(username, service_name, password);

    add_service_to_file(s.map_array[s.service_count-1]);
}

int add_service_to_file(Map *new_service) {
    FILE *f = fopen("pkenc.txt", "a");
    fputs("\n", f);
    fputs(new_service->service_name, f);
    fputs("\n", f);
    fputs(new_service->username, f);
    fputs("\n", f);
    fputs(new_service->password, f);

    fclose(f);
}

int create_map_and_add_to_array(char *username, char *service_name, char *password) {
    Map *new_map = malloc(sizeof(*new_map));
    
    new_map->key = select_key();
    new_map->username = malloc(strlen(username) + 1);
    strcpy(new_map->username, username);

    new_map->service_name = malloc(strlen(service_name) + 1);
    strcpy(new_map->service_name, service_name);

    new_map->password = malloc(strlen(password) + 1);
    strcpy(new_map->password, password);

    s.service_count++;
    s.map_array = realloc(s.map_array, s.service_count*sizeof(Map *));
    s.map_array[s.service_count - 1] = new_map;
    return 0;
}

char select_key(void) {
    if (s.service_count < 10) {
        return 48 + s.service_count;
    } else {
        return 87 + s.service_count;
    }
}

int key_to_index(char key) {
    if (key >= 48 && key <= 58) {
        return key - 48;
    } else if (key >= 87 && key <= 114) {
        return key - 87;
    } else {
        return -1;
    }
      
}

int next_page(void) {
    if (page < s.service_count/8) {
        page++;
    }
}

int prev_page(void) {
    if (page > 0) {
        page--;
    }
}

int run_service(char key) {
    int index = key_to_index(key);
    if (index != -1 && index < s.service_count) {
        display_service(index);
        printf("\n");
        prompt_removal(index);
    }
}



int display_service(int index) {
    
        clear_terminal();
        Map *service = s.map_array[index];

    

        printf("Service: %s\nUsername: %s\nPassword: %s", service->service_name, service->username, service->password);

    return 0;
}

int prompt_removal(int index) {
    char input_s[LINELENGTH];
    printf("Would you like to remove this service? (enter %c-%c for removal, anything else to return to home page)\n", '"', '"');
    /* flush the stdin */
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);

    fgets(input_s, LINELENGTH, stdin);
    input_s[strcspn(input_s, "\n")] = 0;

    if (input_s[0] == '-') {
        remove_service(index);
    } 
    return 0;
}

int remove_service(int index) {
    remove_lines(index);

    int i;

    printf("%s", s.map_array[index]->service_name);
    free(s.map_array[index]);

    for (i = index; i < s.service_count - 1; i++) {
        s.map_array[i] = s.map_array[i + 1];
        if (s.map_array[i]->key == 97) {
            s.map_array[i]->key = 58;
        }
        s.map_array[i]->key--;

    }

    s.service_count--;

    s.map_array = realloc(s.map_array, s.service_count*sizeof(Map *));
}

int remove_lines(int index) {
    int i;
    FILE *ftemp = fopen("temppkenc.txt", "w");
    FILE *f = fopen("pkenc.txt", "r");
    char buffer[LINELENGTH];

    for (i = 0; i < (s.service_count)*3 + 1; i++) {
        fgets(buffer, LINELENGTH, f);
        if (i < index * 3 + 1 || i > (index * 3 + 3)) {
            fputs(buffer, ftemp);
        }
    }

    fclose(ftemp);
    fclose(f);

    remove("pkenc.txt");
    rename("temppkenc.txt", "pkenc.txt");
}