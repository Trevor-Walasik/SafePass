# ifndef SAFE_PASS_STRUCT_DEFS_H

# define SAFE_PASS_STRUCT_DEFS_H

typedef struct map {
    char key;
    char *username;
    char *service_name;
    char *password;
} Map;

typedef struct services {
    Map **map_array;
    int service_count;
} Services;

# endif