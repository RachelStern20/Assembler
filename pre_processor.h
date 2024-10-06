#ifndef PRE_PROCESSOR_H
#define PRE_PROCESSOR_H

#define EXIT_ERROR exit(1)
/**
 * @brief Represents a node in a linked list of macros.
 *
 * The Node structure represents a node in a linked list of macros.
 * Each node contains the name of the macro, the content of the macro, the number of lines in the macro
 * and a pointer to the next node in the list.
 */
typedef struct Node {
    char *macro_name;
    char *macro_contant;
    int macro_contant_line_num;
    struct Node *next;
} Node;

/**
 * @brief Processes the source file for macros and outputs the results to a target file.
 *
 * This function scans the source file for macro definitions and references, processes them,
 * and outputs the results to a target file with the extension ".am".
 *
 * @param source Pointer to the source file to be processed.
 * @param file_name The name of the source file (used for generating the target file name).
 * @param file_nameas The name of the assembly source file.
 * @param head A double pointer to the head of the linked list of macros.
 * The list stores macro definitions encountered in the source file.
 * This list is updated within the function and can be used for further processing.
 * @param am_line_count -counting the number of lunes in am file
 * 
 * @return Returns 0 if the processing is successful, or 1 if an error is encountered.
 */
int macro(FILE *source, char *file_name, char *file_nameas, Node **head,int *am_line_count);

/**
 * @brief Checks the validity of a macro name.
 *
 * This function checks if the given macro name is valid, i.e., it does not exceed
 * the maximum allowed length and is not a reserved keyword or command name.
 *
 * @param str The macro name to check.
 * @param len_macro_name The length of the macro name.
 * @param num_of_line The line number in the file where the macro is defined.
 * @param file_name The name of the file being processed.
 * @param line The line containing the macro definition.
 * @param instructions The array of reserved instructions and directive names.
 * 
 * @return Returns 0 if the macro name is valid, or 1 if it is invalid.
 */
int chek_macro_name(char *str, size_t len_macro_name, int num_of_line, char *file_name, char *line, char *instructions[]);

/**
 * @brief Checks if a given string matches a reserved command name.
 *
 * This function checks if the given string matches any of the reserved command
 * or directive names.
 *
 * @param str The string to check.
 * @param num_of_line The line number in the file where the string is located.
 * @param file_name The name of the file being processed.
 * @param instructions The array of reserved instructions and directive names.
 * 
 * @return Returns 0 if the string is not a reserved name, or 1 if it is a reserved name.
 */
int equal_command_name(char *str, int num_of_line, char *file_name, char *instructions[]);

/**
 * @brief Frees the memory allocated for the linked list of macros.
 *
 * This function frees all the memory allocated for the nodes in the linked list
 * of macros.
 * @param head Pointer to the head of the linked list.
 * The function modifies this pointer to ensure that the caller's pointer is set to NULL after freeing.
 * 
 * @note The function does not return any value.
 */
void free_macro_list(Node **head);

/**
 * @brief Checks for the presence of macro start or end keywords in a line.
 *
 * This function checks if the given line contains the start or end keyword
 * for a macro definition ("macr" or "endmacr").
 *
 * @param line The line to check.
 * @param str_to_find The keyword to search for ("macr" or "endmacr").
 * @param num_of_line The line number in the file being processed.
 * @param file_name The name of the file being processed.
 * 
 * @return Returns 0 if the keyword is not found, or 1 if it is found with invalid syntax.
 */
int is_there_macr_or_endmacr(char *line, char *str_to_find, int num_of_line, char *file_name);

/**
 * @brief Frees memory resources and closes the target file.
 *
 * This function frees the memory allocated for strings and closes the target file.
 *
 * @param count_str The number of strings to be freed.
 * @param file A pointer to the target file to be closed.
 * @param ... A variable number of pointers to strings to be freed.
 * 
 * @note The function does not return any value.
 */
void free_var_resources(int count_str, FILE **file, ...); 

/**
 * @brief Validates the macro definition and checks for errors.
 *
 * This function checks the validity of the macro definition, ensuring the name is
 * not already used and does not conflict with reserved keywords. It also ensures
 * there are no extra characters in the line after the macro name.
 *
 * @param macro_name The name of the macro being defined.
 * @param len_macro_name The length of the macro name.
 * @param num_of_line The line number in the file where the macro is defined.
 * @param file_nameas The name of the file being processed.
 * @param head Pointer to the head of the linked list of macros.
 * @param instructions The array of reserved instructions and directive names.
 * @param error_found Pointer to an integer that will be set to 1 if an error is found.
 * @param macro_def Pointer to an integer that will be set to 1 if the macro definition is started.
 * 
 * @return Returns 0 if the macro definition is valid, or 1 if an error is found.
 */
int validate_macro_definition(char *macro_name, int len_macro_name, int num_of_line, char *file_nameas, Node *head, char *instructions[], int *error_found, int *macro_def);

/**
 * @brief Creates a new macro node and adds it to the linked list.
 *
 * This function creates a new node for a macro and adds it to the linked list
 * of macros.
 *
 * @param macro_name The name of the macro being created.
 * @param len_macro_name The length of the macro name.
 * @param head Pointer to the head of the linked list of macros.
 * @param current Pointer to the current macro node in the linked list.
 * This pointer is updated to point to the new node if it is successfully created.
 *
 * @return Returns a pointer to the newly created macro node, or NULL if memory allocation fails.
 */
Node* create_new_macro(char *macro_name, int len_macro_name, Node **head, Node **current);

#endif
