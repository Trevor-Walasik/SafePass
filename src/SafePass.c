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

# include <aes.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <windows.h>
# include "SafePass.h"
# include "SafePassStructDefs.h"

/* Prints sequence that clears the terminal. */
# define clear_terminal() printf("\033[H\033[2J")
# define print_line_of_dashes() printf("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n")
/* The length of space inbetween a cell in the UI, will be the max for many values in the program. */
# define LINELENGTH 61
# define CREDFILE "pkenc.bin"
uint8_t iv[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
# define reset_iv() uint8_t iv[]  = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };

char c[32];

# define handle_error(error_message) do { \
    perror(error_message); \
    exit(EXIT_FAILURE); \
    } while(0)


/* Services structure that will contain all services in the program. */
Services s = {NULL, 0};
/* int that tracks what page of services the user is looking at in the UI */
int page = 0;

int main(void) {
    
    get_user_key(c);


    /* Create CREDFILE file if not yet created */
    FILE *f = fopen(CREDFILE, "rb");

    if (f == NULL) {
        f = fopen(CREDFILE, "wb");
        if (f == NULL) handle_error("Error opening file");

        fwrite("SafePassUserCredentialsFileDoNotEdit", sizeof(char), 37, f);
    } else {
       populate_s(f);
    }

    fclose(f);

    /* Begin Programs Main Loop */
    main_loop();

    return 0;
}

/* This function will prompt the user to enter an encryption key, it will then store
 * the key in the provided buffer. The key will be padded or truncated to 32 bytes. */
int get_user_key(char *key_buffer) {
    char input[33] = {0}; 
    printf("Enter master key, this will be used to encrypt and decrypt your data.\nMake sure you can remember it because it will not be stored in this program for security.\nEnter encryption key (max 32 characters): ");
    fgets(input, sizeof(input), stdin);

    size_t len = strcspn(input, "\n");
    input[len] = '\0';  
    input[32] = '\0';

    memset(key_buffer, 0, 32);
    strncpy((char *)key_buffer, input, 32);

    return 1;
}



/* This function will read characters from a file until a null terminator is reached,
 * storing the characters in buffer. It returns the number of characters read (excluding
 * the null terminator). */
int read_until_null(FILE *f, char *buffer) {
    int i = 0;
    char ch;

    while (fread(&ch, sizeof(char), 1, f) == 1 && ch != '\0') {
        buffer[i++] = ch;
    }
    buffer[i] = '\0';

    return i;
}


/* This function will be called once at the start of the main loop, if CREDFILE has been open
 * before. The function will read the file line by line, creating Map pointers every 3 lines, and
 * properly filling the fields. The Map pointer will then be added to s. */
int populate_s(FILE *f) {
    char read_buffer[LINELENGTH  + 1];
    /* read first line */
    read_until_null(f, read_buffer);

    while(read_until_null(f, read_buffer) > 0)  {

        Map *new_map = malloc(sizeof(*new_map));
        new_map->key = select_key();
        
        new_map->service_name = malloc(strlen(read_buffer) + 1);
        strcpy(new_map->service_name, read_buffer);

        read_until_null(f, read_buffer);
            
        new_map->username = malloc(strlen(read_buffer) + 1);
        strcpy(new_map->username, read_buffer);

        uint8_t cipher_text[64];
        memset(cipher_text, 0, 64);
        struct AES_ctx ctx;
        reset_iv();
        AES_init_ctx_iv(&ctx, (uint8_t *)c, iv);
        fread(cipher_text, sizeof(char), 64, f);
        
        AES_CBC_decrypt_buffer(&ctx, cipher_text, 64);
        
        new_map->password = malloc(strlen((char *)cipher_text) + 1);
        strcpy(new_map->password, (char *)cipher_text);

        s.service_count++;
        s.map_array = realloc(s.map_array, s.service_count*sizeof(Map *));
        s.map_array[s.service_count - 1] = new_map;
    }
    return 0;
}

/* The main loop will run a simple cycle of clearing the terminal, printing the UI, then awaiting user input.
 * It will then call itself recursively to continue execution. */
int main_loop(void) {
    clear_terminal();

    print_user_interface();

    execute_input();
    
    main_loop();

    return 0;
}   

/* This fucntion will print the basic user interface seen in the main page of the program. */
int print_user_interface(void) {
    print_line_of_dashes();
    printf("|   Enter Services Key: (1, 2, 3, ... , x, y, z) For Password     |\n");
    print_line_of_dashes();
    print_services();
    printf("| < Previous Page |  Enter '+' to Add New Service   | > Next Page |\n");
    print_line_of_dashes();

    return 0;
}

/* Print 8 services padded in cells. Which 8 services are printed is decided by the global page int. */
int print_services(void) {
    int starting_index = page*8;
    int i;

    for (i = starting_index; i < starting_index + 8 && i < s.service_count; i++) {
        seventy_pad(s.map_array[i]->key, s.map_array[i]->service_name);
        print_line_of_dashes();
    }

    return 0;
}

/* Pad a key, service name pair to fit the format of the programs UI. */
int seventy_pad(char key, char *main_string) {
    int pad = LINELENGTH - strlen(main_string);
    int i;
    /* print the key in small cell */
    printf("| %c |", key);

    /* depending on if the string is odd or even length, pad it accordingly using a for
    * loop of spaces. */
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

/* This function will simply get the key entered by the user and execute another 
 * function based on what was inputted. */
int execute_input(void) {
    /* scan for user input and save first char */
    char input_s[LINELENGTH];

    fgets(input_s, sizeof(input_s), stdin);

    check_input_length(input_s);

    input_s[strcspn(input_s, "\n")] = '\0';

    if (input_s[0] == '+') {
        add_credentials();
    } else if (input_s[0] == '>') {
        next_page();
    } else if (input_s[0] == '<') {
        prev_page();
    } else if (input_s[0] == '\t') {
        exit(0);
    } else {
        if (run_service(input_s[0]) == -1) {
            /* Handle invalid input */
            clear_terminal();
            printf("Invalid input. No service for key %c", input_s[0]);
            Sleep(1500);
        }
    }

    return 0;
}

/* This function will take a string and check that it is not too long, it will do so by seeing if a newline 
 * was read by the fgets. If it is, it will post an error message and return -1. */
int check_input_length(char *input_s) {
    /* Check if no newline was found, meaning user input > LINELENGTH */
    if (strchr(input_s, '\n') == NULL) {
        /* flush the stdin */
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);

        clear_terminal();
        printf("Input too long. Maximum allowed length is %d characters.\n", LINELENGTH - 2);
        Sleep(500);

        return -1;
    }

    return 0;
}

/* This function will prompt the user to add a new login service to their password manager.
 * This is done by prompting the user with printf, then using fgets on the stdin to store the
 * string. These will be used to populate a map pointer which will be added to s, and written 
 * in the file. */
int add_credentials(void) {
    /* strings for storing Map strings*/
    char service_name[LINELENGTH];
    char username[LINELENGTH];
    char password[256];
    clear_terminal();

    printf("Enter the name of the new service being added: \n");

    /* Get service name */
    fgets(service_name, LINELENGTH, stdin);

    /* Check that input is not to long */
    while (check_input_length(service_name) == -1) {
        printf("Enter the name of the new service being added: \n");
        fgets(service_name, LINELENGTH, stdin);
    }

    service_name[strcspn(service_name, "\n")] = 0;

    printf("Enter the user name for %s:\n", service_name);

    /* Get username */
    fgets(username, LINELENGTH, stdin);

    /* Check that input is not to long */
    while (check_input_length(username) == -1) {
        printf("Enter the user name for %s:\n", service_name);
        fgets(username, LINELENGTH, stdin);
    }

    username[strcspn(username, "\n")] = 0;

    printf("Enter the password for %s:\n", service_name);

    /* Get password */
    fgets(password, LINELENGTH, stdin);
    password[strcspn(password, "\n")] = 0;

    create_map_and_add_to_array(username, service_name, password);

    add_service_to_file(s.map_array[s.service_count-1]);

    return 0;
}

/* This function will dynamically allocate memory for a new Map pointer, the string for
 * that Map pointers fields, and realloc more space for the s.map_array. */
int create_map_and_add_to_array(char *username, char *service_name, char *password) {
    Map *new_map = malloc(sizeof(*new_map));
    
    new_map->key = select_key();

    /* Allocate memory for username string */
    new_map->username = malloc(strlen(username) + 1);
    strcpy(new_map->username, username);

    /* Allocate memory for service_name string */
    new_map->service_name = malloc(strlen(service_name) + 1);
    strcpy(new_map->service_name, service_name);

    /* Allocate memory for password string */
    new_map->password = malloc(strlen(password) + 1);
    strcpy(new_map->password, password);

    /* Increment service_count and allocate more memory to map array */
    s.service_count++;
    s.map_array = realloc(s.map_array, s.service_count*sizeof(Map *));
    s.map_array[s.service_count - 1] = new_map;

    return 0;
}

/* Write the new service to the end of the file. */
int add_service_to_file(Map *new_service) {
    FILE *f = fopen(CREDFILE, "ab");

    if (f == NULL) handle_error("Error opening file");

    fwrite(new_service->service_name, sizeof(char), strlen(new_service->service_name) + 1, f);
    fwrite(new_service->username, sizeof(char), strlen(new_service->username) + 1, f);

    uint8_t cipher_text[64];
    memset(cipher_text, 0, 64);
    strncpy((char *)cipher_text, new_service->password, LINELENGTH + 1);
    struct AES_ctx ctx;
    reset_iv();
    AES_init_ctx_iv(&ctx, (uint8_t *)c, iv);
    AES_CBC_encrypt_buffer(&ctx, cipher_text, 64);

    fwrite(cipher_text, sizeof(char), 64, f);

    fclose(f);

    return 0;
}

/* This function will determin which key shoul be used and returns it as a char. Keys are:
 * 0, 1, 2, ..., 9, a, b, c, ..., z */
char select_key(void) {
    if (s.service_count < 10) {
        return 48 + s.service_count;
    } else {
        return 87 + s.service_count;
    }
}

/* This function will do the inverse of select key, and instead find what int corresponds to a user
 * inputted char. */
int key_to_index(char key) {
    if (key >= 48 && key <= 58) {
        return key - 48;
    } else if (key >= 87 && key <= 114) {
        return key - 87;
    } else {
        return -1;
    }
      
}

/* increment page int if more servecs can be displayed. */
int next_page(void) {
    if (page < s.service_count/8) {
        page++;

        return 0;
    }

    return -1;
}

/* decrement page int if not on first page. */
int prev_page(void) {
    if (page > 0) {
        page--;

        return 0;
    }

    return -1;
}

/* This function will display to the user credentials for the service they would like to view. */
int run_service(char key) {
    int index = key_to_index(key);

    /* check if user inputted valid key, and display credentials if they did. */
    if (index != -1 && index < s.service_count) {
        display_service(index);
        printf("\n");
        prompt_removal(index); 
        return 0;
    } else {
        return -1;
    }
}

/* This function will simply get the service requested stored in a shallow copy Map pointer,
 * then print to the user the credentials. */
int display_service(int index) {
    clear_terminal();
    Map *service = s.map_array[index];

    printf("Service: %s\nUsername: %s\nPassword: %s", service->service_name, service->username, service->password);

    return 0;
}

/* This function prompts the user to remove the service by entering -. */
int prompt_removal(int index) {
    char input_s[LINELENGTH];
    printf("Would you like to remove this service? (enter %c-%c for removal, anything else to return to home page)\n", '"', '"');

    /* Remove newline */
    fgets(input_s, LINELENGTH, stdin);
    input_s[strcspn(input_s, "\n")] = 0;

    if (input_s[0] == '-') {
        remove_lines(index);
        remove_service(index);
    } 

    return 0;
}

/* This function removes the service from the progams memory. */
int remove_service(int index) {
    int i;

    /* Free memory of the map pointer to be removed */
    free(s.map_array[index]->password);
    free(s.map_array[index]->service_name);
    free(s.map_array[index]->username);
    free(s.map_array[index]);

    /* Shift the key of the map_array Map pointers */
    for (i = index; i < s.service_count - 1; i++) {
        s.map_array[i] = s.map_array[i + 1];
        if (s.map_array[i]->key == 97) {
            s.map_array[i]->key = 58;
        }
        s.map_array[i]->key--;
    }

    /* Decrement service counter and realloc memory for map_array */
    s.service_count--;
    s.map_array = realloc(s.map_array, s.service_count*sizeof(Map *));

    return 1;
}

/* This function will remove the 3 lines of the specified Maps index from the file.
 * This will be done by creating a copy file that is verbatim, besides the index to
 * the index + 3. There is one special edge case where if the final service is being 
 * removed, a newline must be removed from the previous line. */
int remove_lines(int index) {
    int i = 0;
    FILE *ftemp = fopen("temppkenc.txt", "wb");
    FILE *f = fopen(CREDFILE, "rb");

    if (ftemp == NULL) handle_error("Error opening file");
    if (f == NULL) handle_error("Error opening file");

    char buffer[LINELENGTH];

    /* read and write first line to copy */
    read_until_null(f, buffer); 
    fwrite(buffer, sizeof(char), strlen(buffer) + 1, ftemp);

    while(read_until_null(f, buffer) > 0) {
        if (i == index) {
            read_until_null(f, buffer);
            fread(buffer, sizeof(char), 64, f);
        } else {
            fwrite(buffer, sizeof(char), strlen(buffer) + 1, ftemp);
            read_until_null(f, buffer);
            fwrite(buffer, sizeof(char), strlen(buffer) + 1, ftemp);
            fread(buffer, sizeof(char), 64, f);
            fwrite(buffer, sizeof(char), 64, ftemp);
        }
        i++;
    }

    fclose(ftemp);
    fclose(f);

    /* Delete original file */
    remove(CREDFILE);
    /* Rename new file to orginal name */
    rename("temppkenc.txt", CREDFILE);

    return 1;
}