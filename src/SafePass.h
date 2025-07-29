# include "SafePassStructDefs.h"

# ifndef SAFE_PASS_H
# define SAFE_PASS_H

int main_loop(void);
int print_user_interface(void);
int seventy_pad(char key, char *main_string);
int execute_input(void);
int add_credentials(void);
int add_service_to_file(Map *new_service);
int create_map_and_add_to_array(char *username, char *service_name, char *password);
int remove_service(int index);
int next_page(void);
int prev_page(void);
int print_services(void);
char select_key(void);
int key_to_index(char key);
int run_service(char key);
int display_service(int index);
int prompt_removal(int index);
int remove_lines(int index);

# endif