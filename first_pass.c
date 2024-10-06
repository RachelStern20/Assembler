#include "first_pass.h"
#include "second_pass.h"

int first_pass(FILE *file, lable **lable_arr, int *lable_count, int *IC, int *DC, entry_l **entries, code_word *codeword_arr, unknown_lables **unknown_lables_arr, int *unknown_lbl_count, char *file_name, int *data_arr, Node *macro_list,int am_line_count,int * entries_count) 
{
    char *input, *cur_word = NULL, *lable_name;
    char *line_copy = NULL, *original_input = NULL, *original_line_copy = NULL;
    int line_num = 0, lable_flag = 0, index = 0, error = 0;
    int input_size, i,error_command=0;
    size_t len,input_len;
    unknown_lables *temp;

    input=(char*)malloc((MAX_LINE_SIZE+1)*sizeof(char)); /*allocating memory for input -line */
    if(!input)
    {
       printf("\nMemory allocation error\n");
       EXIT_ERROR;
    }
    while (fgets(input,MAX_LINE_SIZE,file)!=NULL && (input[0]!=EOL && input[0]!=EOS))
    {
        line_num++;
        lable_flag = 0;
        index = 0;
        error_command=0;

        if((*IC)+(*DC)>MEMORY_SIZE) /* if all memory used */
        {
           printf("\nERROR from line %d in file %s: Max capacity of memory has been used. next lines will not be read0\n",line_num , file_name);
           error=1;
           break;
         }

        /* rermoving the new lin char if there is */
        input_len=strlen(input);
        if(input[input_len-1]==EOL){
        input[input_len-1]=EOS;
        }
        original_input = input;  /* Save the original input address*/

        /* Allocate memory for line_copy */
        line_copy = (char*)malloc((strlen(input) + 1) * sizeof(char));
        if (!line_copy) 
        {
            printf("\nMemory allocation error\n");  
            EXIT_ERROR;
        }
        strcpy(line_copy, input);
        original_line_copy = line_copy;  /* Save the original line_copy address*/

        /* Process the current word*/
        cur_word = get_current(&input);
        if (cur_word[strlen(cur_word) - 1] == ':') /*if the current  word is lable*/
        {
            cur_word[strlen(cur_word) - 1] = EOS;
            if (check_lable(cur_word, 1, file_name, line_num,macro_list)) 
            {
                error = 1;
                input=original_input;
                continue;
            }
            get_current(&line_copy);  /* Advance line_copy pointer */
            lable_flag = 1;
            lable_name = (char*)malloc(strlen(cur_word) + 1);
            if (!lable_name) 
            {
                printf("\nMemory allocation error\n");
                EXIT_ERROR;
            }
            strcpy(lable_name, cur_word);
            free(cur_word);
            cur_word = get_current(&input);
        }

        /* Handle directives */
        if (!strcmp(cur_word, ".string") || !strcmp(cur_word, ".data")) 
        {
            if (lable_flag) 
            {
                if (update_lables(lable_arr, lable_count, lable_name, INST, IC, DC, file_name, line_num, macro_list)) {
                    error = 1;
                    input=original_input;
                    continue;
                }
            }

            if(strlen(input)==0)
            {
                printf("\nERROR in line %d in file %s - no input after data declaration\n",line_num ,file_name);
                error = 1;
                input=original_input;
                continue;
             }

            if (!strcmp(cur_word, ".data")) 
            {
                if (update_data_nums(&input, DC, data_arr, file_name, line_num)) { /* if data input has errors */
                    error = 1;
                    input=original_input;
                    continue;
                }
            }

            if (!strcmp(cur_word, ".string")) 
            {
                if (valid_str_input(input, file_name, line_num)) {  /* if string input has errors */
                    error = 1;
                    input=original_input;
                    continue;
                }
                update_data_string(&input, DC, data_arr, file_name, line_num); /* updating the data arr with string*/
            }
        } 

        else if (!strcmp(cur_word, ".entry") || !strcmp(cur_word, ".extern")) 
        {
            if (lable_flag) 
            {
                printf("WARNING: label before entry or extern is ignored\n");
            }
            
            if(strlen(input)==0)
            {
                printf("\nERROR in line %d in file %s - no input after lable type declaration\n", line_num, file_name);
                error = 1;
                input=original_input;
                continue;
             }

            if (!strcmp(cur_word, ".entry")) 
            {
                free(cur_word);
                cur_word = get_current(&input);
                if (check_lable(cur_word, 1, file_name, line_num,macro_list)) {
                    error = 1;
                    input=original_input;
                    continue;
                }
                temp= (unknown_lables*)realloc(*unknown_lables_arr,((*unknown_lbl_count)+1)*sizeof(unknown_lables)); 
                lable_name=(char*)malloc(strlen(cur_word)+1); /*alocating memoery for lable_name*/
                if(!temp || !lable_name) 
                {    
                    printf("\nMemory allocation error\n");
                    EXIT_ERROR;
                }
                strcpy(lable_name,cur_word); 
                /* updating unknown lables array with entry (index is -1) */
                (*unknown_lables_arr)=temp;
                (*unknown_lables_arr)[*unknown_lbl_count].name=lable_name;
                (*unknown_lables_arr)[*unknown_lbl_count].index=-1;
                (*unknown_lables_arr)[*unknown_lbl_count].line_num =line_num;
                (*unknown_lbl_count)++;
                update_entries(entries, cur_word, line_num,entries_count);
                len = strlen(input);
                while (index < len) 
                {
                    if (!isspace(input[index])) {
                        printf("\nERROR in line %d in file %s: extraneous text after end\n", line_num, file_name);
                        error = 1;
                        break;
                    }
                    index++;
                }
            }
            else if (!strcmp(cur_word, ".extern"))  
            {
                free(cur_word);
                cur_word = get_current(&input);
                if (update_lables(lable_arr, lable_count, cur_word, EXTERN_TYPE, IC, DC, file_name, line_num, macro_list)) {
                    error = 1;
                    input=original_input;
                    continue;
                }
                while (index < strlen(input)) {
                    if (!isspace(input[index])) {
                        printf("\nERROR in line %d in file %s: extraneous text after end\n", line_num, file_name);
                        error = 1;
                        break;
                    }
                    index++;
                }
            }
        } 

        else if (get_opcode(cur_word) != -1) 
        {
            if (lable_flag) 
            {
                if (update_lables(lable_arr, lable_count, lable_name, COMMAND_TYPE, IC, DC, file_name, line_num, macro_list)) {
                    error = 1;
                    input=original_input;
                    continue;
                }
            }

            /* Process command and free line_copy after usage */
            error_command = command_name(line_copy, line_num, file_name);
            if (error_command) 
            {
                error=1;
                free(cur_word);
                free(original_line_copy);  /* Free the original line_copy address*/
                input=original_input;
                continue;
            }

            input_size = strlen(input);
            for (i = 0; i < input_size; i++) /* taking off ',' */
            { 
                if (input[i] == ',')
                    input[i] = ' ';
            }
            update_code_word(cur_word, &input, codeword_arr, IC, unknown_lables_arr, unknown_lbl_count, file_name, line_num,macro_list); /*updating codword array*/
        }

        else
        {
             error|=1;
             printf("\nERROR in line %d in file %s: undefind command name\n",line_num,file_name);
             free(cur_word);
             free(original_line_copy);
             input=original_input;
             continue;
        }
        free(cur_word);            /* Free the current word */
        free(original_line_copy);  /* Free the original line_copy address*/
        input=original_input;
    }
    free(original_input);      /* Free the original input address */
    return error;
}



char * get_current(char **input) 
{
   int i = 0, j = 0;
   char *cur_check_input, *temp;

   /* Allocating memory */
   cur_check_input = (char *)malloc(sizeof(char)*2);
   if (!cur_check_input) 
   {
      printf("\nMemory allocation error\n");
      EXIT_ERROR;
   }
   cur_check_input[0]=EOS;   

   /* Ignoring spaces in the beginning */
   while (isspace((*input)[i])) 
   { 
      i++;
   }
   
   /* Running until the next space and saving the current word */
   while ((*input)[i] != EOS && !isspace((*input)[i])) 
   {
      temp = (char*)realloc(cur_check_input, (strlen(cur_check_input) + 2)); /* Allocating memory */
      if (!temp) 
      {
         printf("\nMemory allocation error\n");
         EXIT_ERROR;
      }
      cur_check_input = temp;
      cur_check_input[j] = (*input)[i];
      i++;
      j++;
   }
   cur_check_input[j] = EOS;
   
   /* Ignoring spaces in the beginning */
   while (isspace((*input)[i]))    
      i++;
   
   *input = (*input + i);  /* Move the pointer forward in the input string*/
   return cur_check_input;
}

int reserved(char * lable_name ,Node * macro_list) 
{
    int i;
    Node *temp;
    char *reserved_words[] = {
    ".string", ".data", ".entry", ".extern", "mov", "cmp", "add", "sub", "lea",
    "clr", "not", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop",
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"
    };

    for (i = 0; i < 28; ++i) 
    {
        if (strcmp(lable_name, reserved_words[i]) == 0) {
            return 1;
        }
    }

    temp = macro_list;
    while(temp != NULL)
    {
        if(strcmp(temp->macro_name, lable_name) == 0)
        {
            return 2;
        }
        temp = temp->next;
    }
    return 0;
}



int check_lable(char * cur_word, int print_flag,char * file_name ,int line_num,Node * macro_list)
{
    int i;

    if(cur_word==NULL)
    {
        if(print_flag)
           printf("\nERROR in line %d in file %s: no lable found\n ", line_num, file_name);
        return 1;  
    }

    if(strlen(cur_word)==0)
    {   
        if(print_flag)
           printf("\nERROR in line %d in file %s: lable is empty\n", line_num, file_name);
        return 1;  
    }

    if(strlen(cur_word)> MAX_LABLE_SIZE)
    {
        if(print_flag)
           printf("ERROR in line %d in file %s: lable name too long\n", line_num, file_name);
        return 1;
    }
    if(!isalpha(cur_word[0]))
    {
        if(print_flag)
           printf("\nERROR in line %d in file %s: lable does not start with letter\n", line_num, file_name);
       return 1;
    }
    for(i=0;i<strlen(cur_word);i++)
    {
        if(!isdigit(cur_word[i]) && !isalpha(cur_word[i]))
        {
          if(print_flag)
             printf("ERROR in line %d in file %s: lable contains unvalid char\n ", line_num, file_name);
          return 1;  
        }
    }
    if(reserved(cur_word,macro_list)==1) /* if lables name is register name */
    {
        if(print_flag)
           printf("\nERROR in line %d in file %s: lable name is reserved \n", line_num, file_name);
        return 1;  
    }
    if(reserved(cur_word,macro_list)==2) /* if lables name is register name */
    {
        if(print_flag)
           printf("\nERROR in line %d in file %s: lable name is already a macro name \n", line_num, file_name);
        return 1;  
    }
    return 0; /* if no errors */
}

int get_opcode(char* command) 
{
    if (strcmp(command, "mov") == 0) return OPCODE_MOV;
    if (strcmp(command, "cmp") == 0) return OPCODE_CMP;
    if (strcmp(command, "add") == 0) return OPCODE_ADD;
    if (strcmp(command, "sub") == 0) return OPCODE_SUB;
    if (strcmp(command, "lea") == 0) return OPCODE_LEA;
    if (strcmp(command, "clr") == 0) return OPCODE_CLR;
    if (strcmp(command, "not") == 0) return OPCODE_NOT;
    if (strcmp(command, "inc") == 0) return OPCODE_INC;
    if (strcmp(command, "dec") == 0) return OPCODE_DEC;
    if (strcmp(command, "jmp") == 0) return OPCODE_JMP;
    if (strcmp(command, "bne") == 0) return OPCODE_BNE;
    if (strcmp(command, "red") == 0) return OPCODE_RED;
    if (strcmp(command, "prn") == 0) return OPCODE_PRN;
    if (strcmp(command, "jsr") == 0) return OPCODE_JSR;
    if (strcmp(command, "rts") == 0) return OPCODE_RTS;
    if (strcmp(command, "stop") == 0) return OPCODE_STOP;
    return -1;
}

int update_data_nums(char** input, int *DC, int *data_arr,char * file_name ,int line_num) 
{
    int number = 0, i, sign, comma_flag = 1;
    while (**input != EOS) {
        i = 0;
        sign = 1;
        number = 0;

        /* Skip leading spaces at the start of each loop iteration */
        while (isspace((*input)[i])) {
            i++;
        }

        /* Check for sign */
        if ((*input)[i] == '-') {
            sign = -1;
            i++;
        }
        else if ((*input)[i] == '+') {
            sign =1;
            i++;
        }

        /* Check for unnecessary comma */
        if ((*input)[i] == ',' && comma_flag) {
            printf("\nERROR in line %d in file %s: too many commas\n", line_num, file_name);
            return 1;
        }
        comma_flag = 0;

        /* Convert the string to an integer */
        while (isdigit((*input)[i])) {
            number = number * 10 + ((*input)[i] - '0');
            i++;
        }
        number *= sign;

        /* Check for decimal number */
        if ((*input)[i] == '.' && isdigit((*input)[i + 1])) {
            printf("\nERROR in line %d in file %s: not a whole number\n", line_num, file_name);
            return 1;
        }

        /* Check for range */
        if (number < MIN_15BIT_INT || number > MAX_15BIT_INT) {
            printf("\nERROR in line %d in file %s: number out of range\n", line_num, file_name);
            return 1;
        }

        /* Update the  lable_arr with the number */
        data_arr[*DC] = number;
        (*DC)++;

        /* Skip spaces after the number */
        while (isspace((*input)[i])) {
            i++;
        }

        /* Check for characters after the number */
        if ((*input)[i] == ',') {
            comma_flag = 1;
            i++;

            /* Skip spaces after the comma */
            while (isspace((*input)[i])) {
                i++;
            }
            
        } 
        else if ((*input)[i] != EOS) {
            if(isdigit((*input)[i]))
            {
                printf("\nERROR in line %d in file %s: missing camma \n", line_num, file_name);
                return 1;
            }
            else{
              printf("\nERROR in line %d in file %s: invalid char in data input\n", line_num, file_name);
              return 1;
            }
        }

        /* Shorten the string */
        *input += i;
    }
    /* Check for characters after the last character */
    while (isspace(**input)) {
        (*input)++;
    }
    if (**input != EOS) {
        printf("\nERROR in line %d in file %s: invalid trailing characters\n", line_num, file_name);
        return 1;
    }
    return 0;
}

int valid_str_input(char *input, char *file_name, int line_num)
{
    int len;
    int end;
    if (input == NULL)
    {
        printf("\nERROR in line %d in file %s: input is NULL\n", line_num, file_name);
        return 1;
    }
    len = strlen(input);
    if (input[0] != '"') /* if the string does not start with " */
    {
        printf("\nERROR in line %d in file %s: string does not begin with \"\n", line_num, file_name);
        return 1;
    }
    end = len - 1;
    while (end > 0 && isspace(input[end])) /* moving from end until no space*/
    {
        input[end] = EOS;
        end--;
    }
    if (input[end] != '"') /* if there are no " at end */
    {
        printf("\nERROR in line %d in file %s: string does not end with \"\n", line_num, file_name);
        return 1;
    }
    
    return 0;
}

int update_data_string(char** input, int *DC, int *data_arr,char* file_name,int line_num)
{
   int i=1;
   if(valid_str_input(*input,file_name,line_num))
   {
      return 1;
   }
   /* updates the data arr with the chars of the string*/
   while((*input)[i]!='"' && (*input)[i+1]!=EOS)
   {
     data_arr[*DC] = (int)((*input)[i]);
     (*DC)++;
     i++;
   }
   data_arr[*DC]=EOS;    /* adding end of line in the end of string of dada  lable_arr*/
   (*DC)++;
   return 0;
}

void int_to_str(int num, char *str) {
    int i = 0, j;
    char c;

    /* Handle 0 case */
    if (num == 0) {
        str[i++] = '0';
        str[i] = EOS;
        return;
    }

    /* Convert each digit to a character */
    while (num != 0) {
        str[i++] = (num % 10) + '0';
        num /= 10;
    }
    str[i] = EOS;

    /* Reverse the string */
    for (j = 0, i--; j < i; j++, i--) {
        c = str[j];
        str[j] = str[i];
        str[i] = c;
    }
}

void update_entries(entry_l **entries ,char *cur_word, int line_number,int * entries_count)
{
   char* entry_name;
   *entries = (entry_l * )realloc(*entries, sizeof(entry_l)*((*entries_count)+1)); /*making room for another lable*/
   entry_name=(char*)malloc(strlen(cur_word)*sizeof(char)+1); /* allocating for name*/
   
   if (!entry_name || !*entries) /* allocation faild */
   {
       printf("\nMemory allocation error\n");
       EXIT_ERROR;
   }
   strcpy(entry_name,cur_word);
   (*entries)[*entries_count].name=entry_name;
   (*entries)[*entries_count].line_num=line_number;
   (*entries_count)++; /* adding one to entries_count*/
}


int update_lables(lable **lable_arr, int *lbl_count, char *lable_name, int type, int *IC, int *DC,char * file_name ,int line_num,Node * macro_list) 
{  
    int i;
    lable *temp = NULL;
    
    if (check_lable(lable_name, 1,file_name,line_num,macro_list)) {  
        return 1;
    }

    /* Checking if lable already exists */
    for (i = 0; i < (*lbl_count); i++) {
        if (strcmp((*lable_arr)[i].name,lable_name)==0 && type!=4 && (*lable_arr)[i].type!=4) {
            printf("\nERROR in line %d in file %s: repeated lable name\n", line_num, file_name);  
            return 1;
        }
    }
    /* Allocating memory */
    temp = (lable *)realloc(*lable_arr, ((*lbl_count) + 1) * sizeof(lable));
    if (!temp) {
        printf("\nMemory allocation error\n");
        exit (1);  
    }
    *lable_arr = temp;

    /* Allocating memory for label name and copying it */
    (*lable_arr)[*lbl_count].name = malloc(strlen(lable_name) + 1);  
    if (!(*lable_arr)[*lbl_count].name) {  
        printf("\nMemory allocation error\n");
        exit (1);   
    }
    strcpy((*lable_arr)[*lbl_count].name, lable_name);  

    /* Updating label  lable_arr */
    if (type == COMMAND_TYPE) {
        (*lable_arr)[*lbl_count].address = *IC; 
    } 
    else if (type == DATA_TYPE) {
        (*lable_arr)[*lbl_count].address = *DC; 
    } 
    else if(type == EXTERN_TYPE) {
        (*lable_arr)[*lbl_count].address = 0;
    }
    else if(type == ENTRY_TYPE){
        (*lable_arr)[*lbl_count].address = 0;
    }
    (*lable_arr)[*lbl_count].type = type;
    (*lbl_count)++;
    return 0;
}

int num_operands_command(char* cur_word) {
    
    int i;
    /* List of command names */
     char* commands2[5] = {
    "mov", "cmp", "add", "sub", 
    "lea"};
    char* commands1[9] = {"clr", "not", "inc", 
    "dec", "jmp", "bne", "red", 
    "prn", "jsr"};
    char* commands0[2] = {"rts", "stop"};

    for (i = 0; i < 5; i++) {
        if (strcmp(cur_word, commands2[i]) == 0) {
            return 2;  /* Command found */
        }
    }
    for (i = 0; i < 9; i++) {
        if (strcmp(cur_word, commands1[i]) == 0) {
            return 1;  /* Command found */
        }
    }
    for (i = 0; i < 2; i++) {
        if (strcmp(cur_word, commands0[i]) == 0) {
            return 0;  /* Command found */
        }
    }
    return -1;  /* Command not found */
}

int is_register(char* operand) {
    if (strcmp(operand, "*r0") == 0 || strcmp(operand, "r0") == 0) return REGISTER0;
    if (strcmp(operand, "*r1") == 0 || strcmp(operand, "r1") == 0) return REGISTER1;
    if (strcmp(operand, "*r2") == 0 || strcmp(operand, "r2") == 0 ) return REGISTER2;
    if (strcmp(operand, "*r3") == 0 || strcmp(operand, "r3") == 0) return REGISTER3;
    if (strcmp(operand, "*r4") == 0 || strcmp(operand, "r4") == 0) return REGISTER4;
    if (strcmp(operand, "*r5") == 0 || strcmp(operand, "r5") == 0) return REGISTER5;
    if (strcmp(operand, "*r6") == 0 || strcmp(operand, "r6") == 0) return REGISTER6;
    if (strcmp(operand, "*r7") == 0 || strcmp(operand, "r7") == 0) return REGISTER7;
    return -1;
}
void update_code_word(char * command_name,char ** input,code_word * codeword_arr,int * IC,unknown_lables ** unknown_lables_arr, int * unknown_lbl_size,char*file_name,int line_num,Node * macro_list)
{
   int type_operand1,type_operand2;
   char * operand1,*operand2;
   /* if the command has 2 operands */
   if(num_operands_command(command_name)==2)
   {
       operand1=get_current(input);
       operand2=get_current(input);
       type_operand1=type_operand(operand1,OPERAND_ONE,file_name,line_num,macro_list);
       type_operand2=type_operand(operand2,OPERAND_TWO,file_name,line_num,macro_list);
       codeword_arr[*IC].value |= get_opcode(command_name); /* 11-14 bits,0-2*/
       codeword_arr[*IC].value |= (type_operand1<<TO_SOURCE_R);       /* 7-10 bits*/
       codeword_arr[*IC].value |= (type_operand2<<TO_TARGET_R);       /* 3-6 bits*/
       (*IC)++;
       if((type_operand1==INDIRECT_REGISTER || type_operand1==DIRECT_REGISTER)&&(type_operand2==INDIRECT_REGISTER || type_operand2==DIRECT_REGISTER))
       {
          codeword_arr[*IC].value |= (is_register(operand1)<<S_REG_BITS_CODE); /* moving to bit 7-10*/
          codeword_arr[*IC].value |= (is_register(operand2)<<T_REG_BITS_CODE); /* moving to bits 3-6*/
          codeword_arr[*IC].value |= BASIC_ARE;
          (*IC)++;
       }
       else 
       {
            update_operand_codeword(codeword_arr,operand1,IC,type_operand1,OPERAND_ONE,unknown_lables_arr,unknown_lbl_size,line_num);
            (*IC)++;
            update_operand_codeword(codeword_arr,operand2,IC,type_operand2,OPERAND_TWO,unknown_lables_arr,unknown_lbl_size,line_num);
            (*IC)++;
       }
   }

   /* if the command has 1 operand */
   if(num_operands_command(command_name)==1)
   {
        operand1=get_current(input);
        type_operand1=type_operand(operand1,2,file_name,line_num,macro_list);              /* 2 for target opperand*/
        codeword_arr[*IC].value |= get_opcode(command_name); /* 11-14 bits,0-2*/
        codeword_arr[*IC].value |= (type_operand1<<T_REG_BITS_CODE);       /* 3-6 bits*/
        (*IC)++;
        update_operand_codeword(codeword_arr,operand1,IC,type_operand1,OPERAND_TWO,unknown_lables_arr,unknown_lbl_size,line_num);
        (*IC)++;
   }

   /* if the command has 0 operands */
   if(num_operands_command(command_name)==0)
   {
       codeword_arr[*IC].value |= get_opcode(command_name); /* 11-14 bits,0-2*/
       (*IC)++;
   }

}

int type_operand(char * operand,int operand_num,char * file_name,int line_num,Node * macro_list)
{
   if(operand[0]=='#') {
     return EMDIET;
     }
   if(operand[0]=='*'&& is_register(operand)!=-1){
     return INDIRECT_REGISTER;}
   if(is_register(operand)!=-1){
     return DIRECT_REGISTER;}
   if(check_lable(operand,1,file_name,line_num,macro_list)!=1) {
      return DIRECT;}
    return -1;   /*if is not correct input */
}

void update_operand_codeword(code_word * codeword_arr,char* operand ,int * IC,int type_operand,int op_num,unknown_lables ** unknown_lables_arr,int * unknown_lbl_count,int line_num)
{
    unknown_lables * temp;
    int number=0;
    char* lable_name;

    if(type_operand==DIRECT) /* if the opernad is a lable we dont know the address in first pass*/
    {
        codeword_arr[*IC].value=0;
        temp= (unknown_lables*)realloc(*unknown_lables_arr,((*unknown_lbl_count)+1)*sizeof(unknown_lables)); /*alocating menory for another index in array*/
        lable_name=(char*)malloc(strlen(operand)+1); /*allocating memory for the lable name*/
        if(!temp || !lable_name) 
        {    
           printf("\nMemory allocation error\n");
           EXIT_ERROR;
        }
        strcpy(lable_name,operand);
        (*unknown_lables_arr)=temp;
        /*updating the index with lable info*/
        (*unknown_lables_arr)[*unknown_lbl_count].name=operand;
        (*unknown_lables_arr)[*unknown_lbl_count].index=*IC;
        (*unknown_lables_arr)[*unknown_lbl_count].line_num =line_num;
        (*unknown_lbl_count)++;
    }
    else if(type_operand==INDIRECT_REGISTER || type_operand==DIRECT_REGISTER)
    {
        codeword_arr[*IC].value|=(BASIC_ARE); /* basic ARE is 100 */
        if(op_num==2) /* if operand is target operand (operand2) */
        {
           codeword_arr[*IC].value|=(is_register(operand)<<T_REG_BITS_CODE); /* doing or on the right bits with the registers number */
        }
        if(op_num==1) /* if operand is source operand (operand 1) */
        {
           codeword_arr[*IC].value|=(is_register(operand)<<S_REG_BITS_CODE);
        }
    }
    else /* if is a number */
    {
        operand=(operand+1); /* cutting of the #*/
        codeword_arr[*IC].value|=(BASIC_ARE); /* basic ARE is 100 */
        number = atoi(operand);  
        codeword_arr[*IC].value|=(number<<T_REG_BITS_CODE); /* doing or on the right bits with the  number */
    }
}



