#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "pre_processor.h"

int macro(FILE *source, char *file_name, char *file_nameas, Node **head,int * am_line_count)
{
    char *instructions[] = {"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "red", "prn"
    , "jsr", "rts", "stop", ".data", ".string", ".entry", ".extern", "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};
    /* I assumed that a macro name is allowed to be macr
    In such a situation the program will be executed as usual. */ 

    int num_of_line = 0, macro_def = 0, c, line_printed, is_there_macr, error_found = 0,  macro_node_created = 0;
    Node *current = NULL, *macro, *temp;
    FILE *target;
    size_t len_macro_name, line_len, new_len, target_len;
    char *line_skip_spaces, *macro_chek = NULL, *space, *macro_name, *line = NULL, *target_file;
    *am_line_count=0;
   
    target_len = strlen(file_name + 4); /*.am and null terminated*/
    target_file = (char *) malloc(target_len * sizeof(char));
    if(target_file == NULL)
    {
        fprintf(stdout, "\nMemory allocation error\n");
        EXIT_ERROR;
    }

    strcpy(target_file, file_name);
    strcat(target_file, ".am");
    target = fopen(target_file, "w");

    if(!target)
    {
        fprintf(stdout, "\nCan't open am file for file %s\n", file_name);
        free_macro_list(head);
        free(target_file);
        EXIT_ERROR;
    }
    
    while(1)
    {
        line = (char *)malloc(1);
            if(!line)
            {
                fprintf(stdout, "\nMemory allocation error\n");
                free_macro_list(head);
                free_var_resources(1, &target, &target_file);
                EXIT_ERROR;
            }

            num_of_line++;
            line_printed = 0;
            line_len = 0;
       while((c = fgetc(source)) != '\n' && c != EOF) /* Scanning the line */
        {
            line = (char *) realloc(line, line_len + 1);
            if(!line)
            {
                fprintf(stdout, "\nMemory allocation error\n");
                free_macro_list(head);
                free_var_resources(1, &target, &target_file);
                EXIT_ERROR;
            }

            line[line_len++] = c;
            if(line_len > 80)
            {
                fprintf(stdout, "\nError in line %d in file %s: Number of characters in a line is greater than 80\n", num_of_line, file_nameas);
                error_found = 1;
                while ((c = fgetc(source)) != '\n' && c != EOF);
                break;
            }
        }
        if(c == EOF && line_len == 0) 
        {
            free(line);
            break;
        }
        

        line = (char *) realloc(line, line_len + 2); /* \n and \0 */
        if(!line)
            {
                fprintf(stdout, "\nMemory allocation error\n");
                free_macro_list(head);
                free_var_resources(1, &target, &target_file);
                EXIT_ERROR;
            }

        line[line_len++] = '\n';
        line[line_len++] = '\0';

        line_skip_spaces = line;

        if((unsigned char)*line_skip_spaces == ';')
        {
            free(line);
            continue;
        }

        /* Skipping all the white characters before the contant in the line */
        while(isspace((unsigned char)*line_skip_spaces)) 
        {
            line_skip_spaces++;
        }
        
        if(strlen(line_skip_spaces) == 0) /* Empty line or note line */
        {
            free(line);
            continue;
        }


        if(macro_def == 1) /* We are inside macro defenition */
        {
            /* Cheking if this line is end of the macro defenition */
            if(strncmp("endmacr", line_skip_spaces, 7) != 0)/* Cheking if there is endmacr in the line */
            {
                if(is_there_macr_or_endmacr(line_skip_spaces +1, "endmacr", num_of_line, file_nameas))
                {
                    error_found = 1;
                    macro_def = 0;
                    macro_node_created = 0;
                    free(line);
                    continue;
                }
            }
            
            if(strncmp("endmacr", line_skip_spaces, 7) == 0)
            {
                line_skip_spaces += 7;
                while(isspace((unsigned char)*line_skip_spaces)) /* Skipping all the white characters before the contant in the line */
                {
                    line_skip_spaces++;
                }

                if(*line_skip_spaces != '\n' && *line_skip_spaces != '\0')
                {
                    fprintf(stdout, "\nError in line %d in file %s: Prohibited in writing additional characters after macro setting\n", num_of_line, file_nameas);
                    error_found = 1;
                    macro_def = 0;
                    macro_node_created = 0;
                    free(line);
                    continue;
                }
                
                /* Else - the end line is good */
                macro_def = 0;
                free(line);
                continue;
            }

            /* The macro node was not created because of error in macro defenition */
            if(macro_node_created == 0)
            {
                free(line);
                continue;
            }
            
            new_len = strlen(current->macro_contant) + strlen(line) + 1;
            current->macro_contant = (char *) realloc(current->macro_contant, new_len);
            if(!macro->macro_contant)
            {
                fprintf(stdout, "\nMemory allocation error\n");
                free_var_resources(2, &target, &line, &target_file);
                free_macro_list(head);
                EXIT_ERROR;
            }

            strcat(current->macro_contant, line); /* Thread */
            free(line);
            continue; 
        }

        
        temp = *head;
       /* First we will check if the line is the name of an existing macro */
        while(temp != NULL)
        {            
            if(temp->macro_name && strncmp((temp->macro_name), line_skip_spaces, strlen(temp->macro_name)) == 0 && 
            (isspace(*(line_skip_spaces + strlen(temp->macro_name))) || *(line_skip_spaces + strlen(temp->macro_name)) == '\0')) 
            {
                /* Skipping all the white characters before the contant in the line */
                line_skip_spaces += strlen(temp->macro_name);
                while(isspace((unsigned char)*line_skip_spaces)) 
                {
                    line_skip_spaces++;
                }

                if(*line_skip_spaces != '\n' && *line_skip_spaces != '\0') 
                {
                    line_printed = 1;
                    (*am_line_count)++;
                    fprintf(target, "%s", line);
                    break;
                }
                else
                {
                    line_printed = 1;
                    (*am_line_count)+=temp->macro_contant_line_num;
                    fprintf(target, "%s", temp->macro_contant);
                    break;
                }
                
            }
            temp = temp->next;
        }

        /* It's not a macro name - cheking if its a start of a macro defenition */
        if(!line_printed && !macro_def) 
        {
            /* Now we extract the first word in the line */
            macro_chek = (char *) malloc(5 * sizeof(char));

            if(!macro_chek)
            {
                fprintf(stdout, "\nMemory allocation error\n");
                free_var_resources(2, &target, &line, &target_file);
                free_macro_list(head);
                EXIT_ERROR;
            }

            strncpy(macro_chek, line_skip_spaces, 4);
            macro_chek[4] = '\0';

            line_skip_spaces += 4; /* Skipping the macr to chek the macro name */

            /* First word is not macr or it's macr and then straight other charchters */
            if(strcmp(macro_chek, "macr") != 0 
            ||(strcmp(macro_chek, "macr") == 0 && !isspace((unsigned char)*line_skip_spaces) && *line_skip_spaces != '\n')) 
            { 

                /* Cheking if there are other "macr" in the line */
                is_there_macr = is_there_macr_or_endmacr(line_skip_spaces - 3, "macr", num_of_line, file_nameas);
                if(is_there_macr == 0) /* There is no "macr" */
                {
                    fprintf(target, "%s", line);
                    (*am_line_count)++;
                    free(line);
                    free(macro_chek);
                    continue;
                }
                else
                {
                    /* Error in macro defeiniton */
                    error_found = 1;
                    macro_def = 1;
                    free(line);
                    continue; 
                }
            }
            /* Else then its a macro definition */
            
            while(isspace((unsigned char)*line_skip_spaces))
            {
                line_skip_spaces++;
            }

            /* If there isn't a macro name */
            if(strlen(line_skip_spaces) == 0)
            {
                fprintf(stdout, "\nError in macro defenition, line %d in file %s: No macro name was enteredֿ\n", num_of_line, file_nameas);
                error_found = 1;
                macro_def = 1;
                free(line);
                continue;
            }

            space = strchr(line_skip_spaces, ' ');
 
            if(space != NULL) /* There is a space */
            {
                len_macro_name = space - line_skip_spaces;
            }
            else
            {
                /* If there was no space, the string will include the newline character */
                len_macro_name = strlen(line_skip_spaces) - 1; 
            }

            macro_name = malloc((len_macro_name + 1) * sizeof(char));

           if(!macro_name)
           {
              fprintf(stdout, "\nMemory allocation error\n");
              free_var_resources(3, &target, &line, &macro_chek, &target_file);
              free_macro_list(head);
              EXIT_ERROR;
           }
   
            strncpy(macro_name, line_skip_spaces, len_macro_name);
            macro_name[len_macro_name] = '\0';

            if(validate_macro_definition(macro_name, len_macro_name, num_of_line, file_nameas, *head, instructions, &error_found, &macro_def)) 
            {
               free(line);
               free(macro_chek);
               free(macro_name);
               continue;
            }

            /* Cheking is there are extra characters after macro definition */
            line_skip_spaces += strlen(macro_name);
            while(isspace((unsigned char)*line_skip_spaces))
            {
                line_skip_spaces++;
            }

            if(*line_skip_spaces != '\n' && *line_skip_spaces != '\0')
            {
                fprintf(stdout, "\nError in line %d in file %s: Unnecessary characters after defining a macro are prohibited.\n", num_of_line, file_nameas);
                error_found = 1;
                macro_def = 1;
                free(line);
                free(macro_name);
                free(macro_chek);
                continue;
            }

            /* Else - the name and the line is valid */
            macro = create_new_macro(macro_name, len_macro_name, head, &current);
            if (!macro) {
                free_var_resources(4, &target, &line, &macro_name, &macro_chek, &target_file);
                free_macro_list(head);
                EXIT_ERROR;
            }

            macro_def = 1;
            macro_node_created = 1;
            free(macro_name);
            free(macro_chek);

        } 
        else
        {
            free(line);
        }

    }
    fclose(target);

    /* Check if there were any errors and if so delete the am file */
    if(error_found == 1)
    {
        if(remove(target_file) != 0) 
        {
            printf("\nError deleting file\n");
        }
    }
    
    return error_found;
}


int chek_macro_name(char *str, size_t len_macro_name, int num_of_line, char *file_name, char *line, char *instructions[])
{
    
    if(len_macro_name > 31)
    {
        fprintf(stdout, "\nError in line %d in file %s: Length of Macro Name larger than 31\n", num_of_line, file_name);
        return 1;
    }
    
   if(equal_command_name(str, num_of_line, file_name, instructions)) /* We got an error */
   {
        return 1;
   }

   line += len_macro_name; /* Cheking if there is other characters after the name */
   while(isspace((unsigned char)*line))
    {
        line++;
    }

    if(*line != '\n' && *line != '\0')
    {
        fprintf(stdout, "\nError in line %d in file %s: Prohibited in writing additional characters after macro setting\n", num_of_line, file_name);
        return 1;
    }

    /* No errors */
    return 0;
}

int equal_command_name(char *str, int num_of_line, char *file_name, char *instructions[])
{
    int i;
    for(i = 0; i < 28; i++)
    {
        if(strcmp(str, instructions[i]) == 0)
        {
            fprintf(stdout, "\nError in line %d in file %s: Macro name cannot be a teaching/directive/Macro definition\n", num_of_line, file_name);
            return 1;
        }
    }
    return 0;
}

void free_macro_list(Node **head) 
{
    Node *temp;
    while (*head != NULL) {
        temp = *head;
        *head = (*head)->next;
        free(temp->macro_name);
        free(temp->macro_contant);
        free(temp);
    }
    *head = NULL;
}

int is_there_macr_or_endmacr(char *line, char *str_to_find, int num_of_line, char *file_name)
{
    char *search_result;
    search_result = strstr(line, str_to_find);
    while(search_result && *search_result != '\0')
    {
        if(isspace(*(search_result - 1)) && isspace(*(search_result + strlen(str_to_find)))) /* Start or end of macro definiton */
        {
            fprintf(stdout, "\nError in line %d in file %s: Prohibited in writing additional characters before macro setting\n", num_of_line, file_name);
            return 1;
        }
        search_result = strstr(line + strlen(str_to_find), str_to_find);
    }
    return 0;
}

void free_var_resources(int count_str, FILE **file, ...) 
{
    int i;
    va_list args;
    char **str;
    va_start(args, file);

    /* Freeing the strings */
    for (i = 0; i < count_str; i++) 
    {
        str = va_arg(args, char **);
        if (str != NULL)
        {
            if(*str != NULL)
            {
                free(*str);
            }
        }
    }

    /* Closing the target file */
    if(file != NULL && *file != NULL) 
    {
        fclose(*file);
    }
        
    va_end(args);
}

int validate_macro_definition(char *macro_name, int len_macro_name, int num_of_line, char *file_nameas, Node *head, char *instructions[], int *error_found, int *macro_def) 
{
    int name_chek;
    Node *temp;

    name_chek = chek_macro_name(macro_name, len_macro_name, num_of_line, file_nameas, macro_name, instructions); /* A function that checks if the macro name is valid */
    if(name_chek)
    {
        *error_found = 1;
        *macro_def = 1;
        return 1;
    }

    /* Checking if the macro name is an existing macro name */
    temp = head;
    while(temp != NULL) 
    {
        if(strcmp(macro_name, temp->macro_name) == 0) 
        {
            fprintf(stdout, "\nError in macro definition, line %d in file %s: The macro name is the same as an existing macro name, repetition of existing names is prohibited.\n", num_of_line, file_nameas);
            *error_found = 1;
            *macro_def = 1;
            return 1;
        }
        temp = temp->next;
    }

    return 0; /* Valid macro */
}

Node* create_new_macro(char *macro_name, int len_macro_name, Node **head, Node **current) 
{
    Node *macro = (Node *)malloc(sizeof(Node));
    if(!macro) 
    {
        fprintf(stdout, "\nMemory allocation error\n");
        return NULL;
    }

    macro->macro_name = (char *)malloc((len_macro_name + 1) * sizeof(char));
    if(!macro->macro_name) 
    {
        free(macro);
        fprintf(stdout, "\nMemory allocation error\n");
        return NULL;
    }

    strcpy(macro->macro_name, macro_name);
    macro->macro_contant = (char *)malloc(1); /* Initialize with an empty string */
    if(!macro->macro_contant) 
    {
        free(macro->macro_name);
        free(macro);
        fprintf(stdout, "\nMemory allocation error\n");
        return NULL;
    }

    macro->macro_contant[0] = '\0';
    macro->next = NULL;
    macro->macro_contant_line_num=0;
    
    if (*head == NULL) 
    {
        *head = macro;
    } 
    else 
    {
        (*current)->next = macro;
    }

    *current = macro;
    return macro;
}



