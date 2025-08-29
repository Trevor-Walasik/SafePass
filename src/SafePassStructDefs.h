/* Copyright (c) 2025 Trevor Walasik
 * All rights reserved
 *
 * This header file contains the definition for structures that are used in
 * the SafePass program.
 */

# ifndef SAFE_PASS_STRUCT_DEFS_H

# define SAFE_PASS_STRUCT_DEFS_H

/* The map structure stores data about a login service. the char pointers will be 
 * dynamically allocated strings. */
typedef struct map {
    char key;
    char *username;
    char *service_name;
    char *password;
} Map;

/* Fixed-size struct for binary file operations */
typedef struct credential {
    char key;
    char service_name[61];
    char username[61];
    char password[256];
} Credential;

/* services will simply be a dynamically allocated array of map pointers, and a count
 * int that tracks how many services are in that array. */
typedef struct services {
    Map **map_array;
    int service_count;
} Services;

# endif