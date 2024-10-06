/* Functions for checking the correctness of the instructions */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "inst_error_check.h"

int command_name(char* str, int num_of_line, char *file_name)
{

    char *command, *command_without_comma = NULL;
    size_t len_command, len_command_comma;
    char *space, *comma;  
    
    char *instructions[] = {"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "red", "prn"
    , "jsr", "rts", "stop", ".data", ".string", ".entry", ".extern"};
     
    while(isspace(*str))
    {
        str++;
    }

    space = strchr(str, ' '); /* Finding the first space in str */
    comma = strchr(str, ',');
   

    /* If there is no space in the string */  
    if(space == NULL)  
    {
        space = str + strlen(str); /* If there is no space, point space to the end of the string */
    }

    len_command = space - str; /* The command length */

    command = (char *) malloc(len_command + 1); /* Plus 1 - a place to null terminateds */
    
    if(command == NULL) 
    {
        printf("\nMemory allocation error\n"); 
        EXIT_ERROR;
    }

    strncpy(command, str, len_command);
    command[len_command] = '\0'; /* Null terminated the string */

    if(comma != NULL && comma < space) /* There is a comma before the space */
    {
        len_command_comma = comma - str; /* The command without the comma */
        command_without_comma = (char *) malloc(len_command_comma + 1);
        if(command_without_comma == NULL) 
        {
            printf("\nMemory allocation error\n");
            free(command);  
            EXIT_ERROR;
        }

        strncpy(command_without_comma, str, len_command_comma); /* Copy the command without comma to command */
        command_without_comma[len_command_comma] = '\0'; /* Null-terminate the command_without_comma string */

        if(same_as_directive_or_instruction(command_without_comma, 0, 16, instructions) == 1)
        {
            /* The name of the command is correct so the comma is a problom */
            printf("\nERROR in line %d in file %s: Illegal comma after command\n", num_of_line, file_name); 
            free(command); 
            free(command_without_comma); 
            return 1;
        }
        else
        {
            printf("\nERROR in line %d in file %s: Undefined command name\n", num_of_line, file_name); /* The command name is not legal */
            free(command);
            free(command_without_comma);
            return 1;
        }
    }

    /* If the user enterd only command name */
    if (*space == '\0') 
    {

        if(command_without_comma == NULL)
        {

            if(same_as_directive_or_instruction(command, 14, 16, instructions) == 1)
            {
                free(command);
                return 0;
            }

            if(same_as_directive_or_instruction(command, 0, 5, instructions) == 1)
            {
                printf("\nERROR in line %d in file %s: Missing source operand after command\n", num_of_line, file_name);
                free(command);
                return 1;
            }

            else if(same_as_directive_or_instruction(command, 5, 14, instructions) == 1)
            {
                printf("\nERROR in line %d in file %s: Missing target operand after command\n", num_of_line, file_name);
                free(command);
                return 1;

            }

            printf("\nERROR in line %d in file %s: Undefined command name\n", num_of_line, file_name);
            free(command);
            return 1;
        }

        /* Commands that dont need arguments */
        if(same_as_directive_or_instruction(command_without_comma, 14, 16, instructions) == 1)
        {

            free(command);
            free(command_without_comma);
            return 0;
        }

        if(same_as_directive_or_instruction(command_without_comma, 0, 14, instructions) == 1)
        {
            printf("\nERROR in line %d in file %s: Missing parameter\n", num_of_line, file_name);
            free(command_without_comma);
            free(command);
            return 1;
        }
        printf("\nERROR in line %d in file %s: Undefined command name\n", num_of_line, file_name);
        free(command);
        return 1;
    }
    
    /* If the user enterd one of the two arguments commonds */
    if(same_as_directive_or_instruction(command, 0, 5, instructions))
    {
        if (*space != '\0')
        {
            /* Sending to a function that cheks if there are no errors for the two arguments commands */
            return check_two_arguments(space+1, command, num_of_line, file_name, instructions); 

           /* Sending starting with the character after the space */
        }
    }

    /* If the user enterd one of the one arguments commonds */
    else if(same_as_directive_or_instruction(command, 5, 14, instructions) == 1)
    {
        if (*space != '\0')
        {
            /* Sending to a function that cheks if there are no errors for the one argument commands */
            return check_one_argument(space+1, command, num_of_line, file_name, instructions); 
        }
    }
    
    /* If the user enterd one of the none arguments commonds */
    else if(same_as_directive_or_instruction(command, 14, 16, instructions) == 1)
    {
        if (*space != '\0')
        {
              str += len_command;
               while(isspace(*str))
                {
                    str++;
                }

                if(!isspace(*str) && *str != '\0')
                {
                    printf("\nERROR in line %d in file %s: Extraneous text after end of command\n", num_of_line, file_name);
                    free(command);
                    free(command_without_comma);
                    return 1;
                }
                free(command);
                free(command_without_comma);
                return 0;
        }
    }

    /* If the command does not equal to any of the commads */
    else
    {
        printf("\nERROR in line %d in file %s: Undefined command name\n", num_of_line, file_name);
        free(command);
        return 1;
    }

    free(command);
    return 0;
}


int check_two_arguments(char *str, char *command_name, int num_of_line, char *file_name, char *instructions[])
{

    char *space, *comma;
    char *first_operand, *second_operand;
    size_t len_first_operand = 0, len_second_operand = 0;
    
    if(*str == ',')
    { 
        printf("\nERROR in line %d in file %s: Illegal comma after command\n", num_of_line, file_name);
        free(command_name);
        return 1;
    }

    while(isspace(*str))
    {
        str++;
    }

    if(*str == '\0')/* End of the string */
    {
        printf("\nERROR in line %d in file %s: Missing source operand after command\n", num_of_line, file_name);
        free(command_name);
        return 1;
    }

    space = strchr(str, ' ');
    comma = strchr(str, ',');

    /* Cheking the first operand length */
    if(space == NULL && comma == NULL)
    {
        len_first_operand = strlen(str);
    }
    else if((space == NULL && comma != NULL) ||(comma != NULL && comma < space))
    {
        len_first_operand = (comma - str);
    }
    else if((space != NULL && comma == NULL) || (space != NULL && space < comma))
    {
        len_first_operand = (space - str);
    }

    first_operand = (char *) malloc(len_first_operand+ 1);

    if(first_operand == NULL)
    {
        printf("\nMemory allocation error\n");
        EXIT_ERROR;
    }

    strncpy(first_operand, str, len_first_operand); 
    first_operand[len_first_operand] = '\0'; /* Null terminated the first operand*/

    str += len_first_operand; /* Skipping the first operand in str */
    
    /*--------------------------------------- Cheking the validation of the first operand ----------------------------------------*/
    /* Send to the function that checks the correctness of the first operand */
   if(chek_first_operand_validation(first_operand, command_name, num_of_line, file_name, instructions) == 1)
   {
        free(command_name);
        free(first_operand);
        return 1;
   }
   else
   {
        free(first_operand);
   }
    /*--------------------------------- End of checking the first operand - the source operand -----------------------------------*/

    /* Continue to check the integrity of the string */
    while(isspace(*str))
    {
        str++;
    }

    if(*str != ',')
    {
        printf("\nERROR in line %d in file %s: Missing comma after the first operand\n", num_of_line, file_name); /*if the first operand is valid*/
        free(command_name);
        return 1;
    }

    /* If we have a comma after the first operand */
    if(*str == '\0' || *(str + 1) == '\0')
    {
        printf("\nERROR in line %d in file %s: Missing second operand - destination operand\n", num_of_line, file_name);
        free(command_name);
        return 1;
    }
    str++;

    while(isspace(*str))
    {
        str++;
    }

    if(*str == ',')
    {
        
        printf("\nERROR in line %d in file %s: Multiple consecutive commas after the first operand\n", num_of_line, file_name); /*if the first operand is valid*/
        free(command_name);
        return 1;
    }

    if(*str == '\0')
    {
        printf("\nERROR in line %d in file %s: Missing second operand - destination operand\n", num_of_line, file_name);
        free(command_name);
        return 1;
    }

    /*-------------------------------- Cheking the second operand - the destination operand -----------------------------------*/
    while(isspace(*str))
    {
        str++; /* Skipping all the spaces before the second operand */
    }

    comma = strchr(str, ',');
    space = strchr(str, ' ');

    len_second_operand = strlen(str);

    /* Cheking the second operand length */
    if((space == NULL && comma != NULL) || (comma != NULL && comma < space))
    {
        len_second_operand = comma - str;
    }

    if((comma == NULL && space != NULL) || (space != NULL && space < comma))
    {
        len_second_operand = space - str;
    }

    second_operand = (char *) malloc(len_second_operand + 1);

    if(second_operand == NULL)
    {
        printf("\nMemory allocation error\n");
        free(command_name);
        EXIT_ERROR;
    }

    strncpy(second_operand, str, len_second_operand);
    second_operand[len_second_operand] = '\0'; /* Null terminated the second operand */

    str += len_second_operand; /* Skipping the second operand in str */

    while(isspace(*str))
    {
        str++;
    }

    if(*str != '\0')
    {
        printf("\nERROR in line %d in file %s: Extraneous text after end of command\n", num_of_line, file_name);
        free(second_operand);
        free(command_name);
        return 1;
    }

    if(chek_second_operand_validation(second_operand, command_name, num_of_line, file_name, instructions) == 1)
    {
        free(second_operand);
        free(command_name);
        return 1;
    }
    return 0;
}

int check_one_argument(char *str, char *command_name, int num_of_line, char *file_name, char *instructions[])
{
    char *space, *comma, *operand;
    size_t len_operand = 0;

    if(*str == ',')
    { 
        printf("\nERROR in line %d in file %s: Illegal comma after command\n", num_of_line, file_name);
        return 1;
    }

    while(isspace(*str))
    {
        str++;
    }

    if(*str == '\0')/* End of the string */
    {
        printf("\nERROR in line %d in file %s: Missing target operand after command\n", num_of_line, file_name);
        free(command_name);
        return 1;
    }

    space = strchr(str, ' ');
    comma = strchr(str, ',');

    /* Cheking the length of the operand */
   if(space == NULL && comma == NULL)
    {
        len_operand = strlen(str);
    }
    else if((space == NULL && comma != NULL) ||(comma != NULL && comma < space))
    {
        len_operand = (comma - str);
    }
    else if((space != NULL && comma == NULL) || (space != NULL && space < comma))
    {
        len_operand = (space - str);
    }

    operand = (char *) malloc(len_operand + 1);

    if(operand == NULL)
    {
        printf("\nMemory allocation error\n");
        EXIT_ERROR;
    }

    strncpy(operand, str, len_operand);
    operand[len_operand] = '\0'; /* Null terminated the operand */

    str += len_operand; /* Skipping the operand to chek the rest of the string */

    if(chek_operand_validation(operand, command_name, num_of_line, file_name, instructions) == 1) /* The operand is not valid */
    {
        free(command_name);
        free(operand);
        return 1;
    }

    /* If the operand is valid */

    while(isspace(*str))
    {
        str++;
    }

    if(*str != '\0') /* We have char after the command */
    {
        printf("\nERROR in line %d in file %s: Extraneous text after end of command\n", num_of_line, file_name);
        free(command_name);
        free(operand);
        return 1;
    }

    free(command_name);
    free(operand);
    return 0;
}


int same_as_directive_or_instruction(char *str, int start, int finish, char *instructions[])
{
    int j;
    for(j = start; j < finish; j++)
    {
        if(strcmp(str, instructions[j]) == 0)
        {
            return 1;
        }
    }
    return 0;
}

/* The following 2 auxiliary functions refer to commands with 2 operands */

int chek_first_operand_validation(char *str, char *command_name, int num_of_line, char *file_name, char *instructions[])
{
    /*------------------- The first thing is to check if it is an addressing method 0 - an immediate number ------------------*/
    if(*str == '#') 
    {
        /* The source operand in the lea method cannot be in the 0 addressing method */
        if(strcmp(command_name, "lea") == 0)
        {
            printf("\nERROR in line %d in file %s: The source operand in the lea command can only be of label type and not of immediate number type, label name can't start with '#'.\n", num_of_line, file_name);
            return 1;
        }

        /* If this is not the lea command, we will check that indeed all the characters after the # represent a whole number */
        str++;
        if(*str == '-'  || *str == '+')
        {
            /* '-' or '+' can appear only the first character after the # */
            str++; 
        }
        
        /* Go through the entire string to check that a whole number does appear after the # character */
        while(*str != '\0')
        {
            if(!isdigit((unsigned char)*str))
            {
                printf("\nERROR in line %d in file %s: Error in writing an immediate address - failure to appear a whole number after the character # is prohibited\n", num_of_line, file_name);
                return 1;
            }
            str++;
        }

        /* A whole number appears after the # character */
        return 0;
    }
    
    /*---------------------------- Checking if it's an addressing method 2 - Indirect register -----------------------------*/
    if(*str == '*') 
    {
    
        if(strcmp(command_name, "lea") == 0)
        {
            printf("\nERROR in line %d in file %s: The source operand in the lea command can only be of label type and not of indirect register type, label name can't start with '*'.\n", num_of_line, file_name);
            return 1;
        }

        if(*(str + 1) != 'r')
        {
            printf("\nERROR in line %d in file %s: Error writing an indirect register - an indirect register can only be * and then r and then of the numbers 0 to 7 with no additional characters after. Additionally a label cannot start with '*'.\n", num_of_line, file_name);
            return 1;
        }

        /* IlLegal indirect registe*/
        if((strncmp(str, "*r", 2) == 0 && !isdigit(*(str+2))) || (strncmp(str, "*r", 2) == 0 && *(str+2) != '0' && *(str+2) != '1' &&
        *(str+2) != '2' && *(str+2) != '3' && *(str+2) != '4' && *(str+2) != '5' &&
        *(str+2) != '6' && *(str+2) != '7') || (strncmp(str, "*r", 2) == 0 && *(str+3) != '\0' && *(str+3) != '\n'))
        {
            printf("\nERROR in line %d in file %s: Error writing an indirect register - an indirect register can only be *r and then one of the numbers 0 to 7 with no additional characters after. Additionally a label cannot start with '*'.\n", num_of_line, file_name);
            return 1;
        }
        
        /* The addressing method is correct */
        return 0;
    }

    /*---------------------------- Cheking if it's an addressing method 3 - Direct register -------------------------------*/
    if(strcmp(str, "r0") == 0 || strcmp(str, "r1") == 0 || strcmp(str, "r2") == 0 || strcmp(str, "r3") == 0 ||
    strcmp(str, "r4") == 0 || strcmp(str, "r5") == 0 || strcmp(str, "r6") == 0 || strcmp(str, "r7") == 0)
    {
        if(strcmp(command_name, "lea") == 0)
        { 
            printf("\nERROR in line %d in file %s: The source operand in the lea method can only be a direct addressing method - a label and cannot be a direct register addressing method.\n", num_of_line, file_name);
            return 1;
        }
        return 0;
    }

    /*----------------------------- Testing the correctness of addressing method 1 - label ------------------------------*/
    
    /* Send to a function that checks whether the label name an instruction or directive name */
    if(same_as_directive_or_instruction(str, 0, 20, instructions) == 1)
    {
        printf("\nERROR in line %d in file %s: Calling a label with an invalid name - label name cannot be an instruction or directive name\n", num_of_line, file_name);
        return 1;
    }

    /* The label name does not begin with an alphabetic letter */
    if(!(isupper(*str)) && !(islower(*str)))
    {
        printf("\nERROR in line %d in file %s: Calling a label with an invalid name - label name have to start with an alphabetic letter\n", num_of_line, file_name);
        return 1;
    }

    /* Go through the operand to check that the label name is correct */
    while(*str != '\0')
    {
        if(!(isupper(*str)) && !(islower(*str)) && !isdigit(*str) && *str != '\n')
        {
            printf("\nERROR in line %d in file %s: Calling a label with an invalid name - The appearance of characters that are not a number or an alphabetic letter in the name of a label is prohibited\n", num_of_line, file_name);
            return 1;
        }
        str++;
    }
    return 0;
}


int chek_second_operand_validation(char *str, char *command_name, int num_of_line, char *file_name, char *instructions[])
{
    /*------------------- The first thing is to check if it is an addressing method 0 - an immediate number ------------------*/
    if(*str == '#')
    {
        /* Only in the cmp function, the destination operand can be in an immediate addressing method */
        if(strcmp(command_name, "cmp") != 0)
        {
            printf("\nERROR in line %d in file %s: Error in the target operand addressing method - the target operand in the %s command cannot be an immediate address - a number.\n", num_of_line, file_name, command_name);
            return 1;
        }
        /* Else - it's the cmp command, cheking the validation of the number */
        str++;
        if(*str == '-' || *str == '+')
        {
            /* '-' or '+' can appear only the first character after the # */
            str++; 
        }
        
        /* Go through the entire string to check that a whole number does appear after the # character */
        while(*str != '\0')
        {
            if(!isdigit((unsigned char)*str))
            {
                printf("\nERROR in line %d in file %s: Error in writing an immediate address - failure to appear a whole number after the character # is prohibited\n", num_of_line, file_name);
                return 1;
            }
            str++;
        }
        /* If the number is valid */
        return 0;
    }

    /*---------------------------- Checking if it's an addressing method 2 - Indirect register -----------------------------*/
    if(*str == '*')
    {
        if(*(str + 1) != 'r')
        {
            printf("\nERROR in line %d in file %s: Error writing an indirect register - an indirect register can only be * and then r and then of the numbers 0 to 7 with no additional characters after. Additionally a label cannot start with '*'.\n", num_of_line, file_name);
            return 1;
        }

        /* IlLegal indirect register */
        if((strncmp(str, "*r", 2) == 0 && !isdigit(*(str+2))) || (strncmp(str, "*r", 2) == 0 && *(str+2) != '0' && *(str+2) != '1' &&
        *(str+2) != '2' && *(str+2) != '3' && *(str+2) != '4' && *(str+2) != '5' &&
        *(str+2) != '6' && *(str+2) != '7') || (strncmp(str, "*r", 2) == 0 && *(str+3) != '\0' && *(str+3) != '\n'))
        {
            printf("\nERROR in line %d in file %s: Error writing an indirect register - an indirect register can only be *r and then one of the numbers 0 to 7 with no additional characters after. Additionally a label cannot start with '*'.\n", num_of_line, file_name);
            return 1;
        }

        /* The addressing method is correct */
        return 0;
    }

    /*---------------------------- Cheking if it's an addressing method 3 - Direct register -------------------------------*/
    if(strcmp(str, "r0") == 0 || strcmp(str, "r1") == 0 || strcmp(str, "r2") == 0 || strcmp(str, "r3") == 0 ||
    strcmp(str, "r4") == 0 || strcmp(str, "r5") == 0 || strcmp(str, "r6") == 0 || strcmp(str, "r7") == 0)
    {
        return 0;
    }

    /*----------------------------- Testing the correctness of addressing method 1 - label ------------------------------*/

   /* Send to a function that checks whether the label name an instruction or directive name */
    if(same_as_directive_or_instruction(str, 0, 20, instructions) == 1)
    {
        printf("\nERROR in line %d in file %s: Calling a label with an invalid name - label name cannot be an instruction or directive name\n", num_of_line, file_name);
        return 1;
    }

    /* The label name does not begin with an alphabetic letter */
    if(!(isupper(*str)) && !(islower(*str)))
    {
        printf("\nERROR in line %d in file %s: Calling a label with an invalid name - label name have to start with an alphabetic letter\n", num_of_line, file_name);
        return 1;
    }

    /* Go through the operand to check that the label name is correct */
    while(*str != '\0')
    {
        if(!(isupper(*str)) && !(islower(*str)) && !isdigit(*str) && *str != '\n')
        {
            printf("\nERROR in line %d in file %s: Calling a label with an invalid name - The appearance of characters that are not a number or an alphabetic letter in the name of a label is prohibited\n", num_of_line, file_name);
            return 1;
        }
        str++;
    }
    return 0;
}

/* This function refer to the one argumen commands */
int chek_operand_validation(char *str, char *command_name, int num_of_line, char *file_name, char *instructions[])
{
    /*------------------- The first thing is to check if it is an addressing method 0 - an immediate number ------------------*/
    if(*str == '#')
    {
        /* Only in the prn function, the destination operand can be in an immediate addressing method */
        if(strcmp(command_name, "prn") != 0)
        {
            printf("\nERROR in line %d in file %s: Error in the target operand addressing method - the target operand in the %s command cannot be an immediate address - a number.\n", num_of_line, file_name, command_name);
            return 1;
        }
        /* Else - it's the prn command, cheking the validation of the number */
        str++;
        if(*str == '-' || *str == '+')
        {
            /* '-' or '+' can appear only the first character after the # */
            str++; 
        }
        
        /* Go through the entire string to check that a whole number does appear after the # character */
        while(*str != '\0')
        {
            if(!isdigit((unsigned char)*str))
            {
                printf("\nERROR in line %d in file %s: Error in writing an immediate address - failure to appear a whole number after the character # is prohibited\n", num_of_line, file_name);
                return 1;
            }
            str++;
        }
        /* If the number is valid */
        return 0;
    }

    /*---------------------------- Checking if it's an addressing method 2 - Indirect register -----------------------------*/
    if(*str == '*')
    {
        if(*(str + 1) != 'r')
        {
            printf("\nERROR in line %d in file %s: Error writing an indirect register - an indirect register can only be * and then r and then of the numbers 0 to 7 with no additional characters after. Additionally a label cannot start with '*'.\n", num_of_line, file_name);
            return 1;
        }
        
        /* IlLegal indirect register */
        if((strncmp(str, "*r", 2) == 0 && !isdigit(*(str+2))) || (strncmp(str, "*r", 2) == 0 && *(str+2) != '0' && *(str+2) != '1' &&
        *(str+2) != '2' && *(str+2) != '3' && *(str+2) != '4' && *(str+2) != '5' &&
        *(str+2) != '6' && *(str+2) != '7') || (strncmp(str, "*r", 2) == 0 && *(str+3) != '\0' && *(str+3) != '\n'))
        {
            printf("\nERROR in line %d in file %s: Error writing an indirect register - an indirect register can only be *r and then one of the numbers 0 to 7 with no additional characters after. Additionally a label cannot start with '*'.\n", num_of_line, file_name);
            return 1;
        }

        /* The addressing method is correct */
        return 0;
    }

    /*---------------------------- Cheking if it's an addressing method 3 - Direct register -------------------------------*/
    if(strcmp(str, "r0") == 0 || strcmp(str, "r1") == 0 || strcmp(str, "r2") == 0 || strcmp(str, "r3") == 0 ||
    strcmp(str, "r4") == 0 || strcmp(str, "r5") == 0 || strcmp(str, "r6") == 0 || strcmp(str, "r7") == 0)
    {
        if(strcmp(command_name, "jmp") == 0 ||strcmp(command_name, "bne") == 0 || strcmp(command_name, "jsr") == 0)
        { 
            printf("\nERROR in line %d in file %s: The source operand in the %s method can only be a direct addressing method or indirect register method - a label and cannot be a direct register addressing method.\n", num_of_line, file_name, command_name);
            return 1;
        }
        return 0;
    }

    /*----------------------------- Testing the correctness of addressing method 1 - label ------------------------------*/

   /* Send to a function that checks whether the label name an instruction or directive name */
    if(same_as_directive_or_instruction(str, 0, 20, instructions) == 1)
    {
        printf("\nERROR in line %d in file %s: Calling a label with an invalid name - label name cannot be an instruction or directive name\n", num_of_line, file_name);
        return 1;
    }

    /* The label name does not begin with an alphabetic letter */
    if(!(isupper(*str)) && !(islower(*str)))
    {
        printf("\nERROR in line %d in file %s: Calling a label with an invalid name - label name have to start with an alphabetic letter\n", num_of_line, file_name);
        return 1;
    }

    /* Go through the operand to check that the label name is correct*/
    while(*str != '\0')
    {
        if(!(isupper(*str)) && !(islower(*str)) && !isdigit(*str) && *str != '\n')
        {
            printf("\nERROR in line %d in file %s: Calling a label with an invalid name - The appearance of characters that are not a number or an alphabetic letter in the name of a label is prohibited\n", num_of_line, file_name);
            return 1;
        }
        str++;
    }
    return 0;
}


