#ifndef FIRST_PASS
#define FIRST_PASS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "inst_error_check.h"
#include "pre_processor.h"

#define EXIT_ERROR exit(1)
#define OPCODE_MOV  0x4
#define OPCODE_CMP  0x804
#define OPCODE_ADD  0x1004
#define OPCODE_SUB  0x1804
#define OPCODE_LEA  0x2004
#define OPCODE_CLR  0x2804
#define OPCODE_NOT  0x3004
#define OPCODE_INC  0x3804
#define OPCODE_DEC  0x4004
#define OPCODE_JMP  0x4804
#define OPCODE_BNE  0x5004
#define OPCODE_RED  0x5804
#define OPCODE_PRN  0x6004
#define OPCODE_JSR  0x6804
#define OPCODE_RTS  0x7004
#define OPCODE_STOP 0x7804
#define MIN_15BIT_INT -16384
#define MAX_15BIT_INT 16383
#define EMDIET 0x1
#define DIRECT 0x2
#define INDIRECT_REGISTER 0x4
#define DIRECT_REGISTER 0x8
#define BASIC_ARE 0x4
#define EXTERN_ARE 0x1
#define LABLE_ARE 0x2
#define NUM_RESERVED_WORDS 28
#define REGISTER0  0x0
#define REGISTER1  0x1
#define REGISTER2  0x2
#define REGISTER3  0x3
#define REGISTER4  0x4
#define REGISTER5  0x5
#define REGISTER6  0x6
#define REGISTER7  0x7
#define MAX_LINE_SIZE 80
#define EOS '\0'
#define EOL '\n'
#define MAX_LABLE_SIZE 30
#define ENTRY_TYPE 4
#define EXTERN_TYPE 3
#define INST 2
#define COMMAND_TYPE 1
#define DATA_TYPE 2
#define OPERAND_ONE 1
#define OPERAND_TWO 2
#define MEMORY_SIZE 3996
#define TO_SOURCE_R 7
#define TO_TARGET_R 3
#define S_REG_BITS_CODE 6
#define T_REG_BITS_CODE 3

typedef struct
{
    char *name;
    int address;
    int type;
} lable;

typedef struct
{
    char *name;
    int line_num;
} entry_l;

/* making struct for code_word with 15 bits */
typedef struct   {
    unsigned int value : 15;
}code_word;

typedef struct 
{
    char * name;
    int index;
    int line_num;
}unknown_lables;     

/* reads line from file and puts it in input */
void get_line(char ** input,FILE * file);

/**
 * @param input a poinetr to string that is gotten from file
 * @return a string with current word (untill space) 
 * @discription: Function gets input and returns string untill last space cuts input to next index without space skips spaces in begning
 * */ 
char * get_current(char ** input);

/**
* @param file the file to read from
* @param lable_arr a pointer to pointer (will do malloc) to an array of lables that are in input
* @param lable_count a pointer to the amount af lables
* @param IC Instruction Count
* @param DC Data Count
* @param entries an array for entry lables that are in first pass
* @param codeword_arr an array for the codwords
* @param unknown_lables_arr an array for all the lables that their codeword value  is unknown
* @param unknown_lbl_count the amount of lables that are in unknown_lables_arr
* @param file_name the current file name for error print
* @param data_arr the array for all data codewords
* @param macro_list a list of all the macros for checking lable name
* @param am_line_count the amount of lines in am file
* @return returns 1 if error found in first pass else returns 0
* @discription: Function is in charge of first pass and updating all info arrays if error occure prints erroe and in the end returns 1
* */
int first_pass(FILE *file, lable **lable_arr, int *lable_count, int *IC, int *DC, entry_l **entries, code_word *codeword_arr, unknown_lables **unknown_lables_arr, int *unknown_lbl_count, char *file_name,int *data_arr, Node * macro_list, int am_line_count,int * entries_count);

/**
 * @param lable_name - the name of the lable to be updated
 * @param type - if befor command type=1 if before .date/.string type=2 if is extern type=3
 * @param DC - adress for the data array info
 * @param IC - adress for the command array info
 * @param lable_arr - the array that contains the lables
 * @param lbls_count - the amount of indexes in lable_arr when gets into function
 * @return if error found returns 1 else 0
 * @param file_name - the name of the current file for error print
 * @param line_num - the number of line in witch the check exicts for error print
 * @param macro_list for to check if lable name is macro name
 * @discription:Function get all parameters and updates lable_arr with them if the lable already exicts returns 1 and prints error
 */
int update_lables(lable **lable_arr, int *lbl_count, char *lable_name, int type, int *IC, int *DC,char * file_name ,int line_num, Node * macro_list);

/**
 * @param input - a pointer to the string that contains the line from file
 * @param DC - a pointer tonumber of Data Count and is used forcurrent index
 * @param data_arr - a pointer to the array that contains the lables 
 * @return if error found returns 1 else 0
 * @discription: Function gets input checks if the input is valid if not prints error if yes updated arr
 */
int update_data_nums(char** input, int *DC, int *data_arr,char * file_name ,int line_num);

/**
 * @param input - a pointer to the string that contains the line from file
 * @param DC - a pointer tonumber of Data Count and is used forcurrent index
 * @param data_arr - a pointer to the array that contains the lables
 * @return if error found returns 1 else 0 
 * @discruption: updatates the data array with the givin data if valid if not valid prints the error with line num and cur file name
 */
int update_data_string(char** input, int *DC, int *data_arr,char *file_name,int line_num);

/**
 * @param input - a pointer to the string that contains the line from file
 * @return checks validtion input if unvalid returns 1 else 0
 * @disctiption: checks if the input id valid if is returns 0 if not prints error end returns 1
 */
int valid_str_input(char *input,char * file_name ,int line_num);

/**
 * @param cur_word - lable to check
 * @param print_flag - if 1 prints else just check validation 
 * @param macro_list - macro list to make sure has different names
 * @return if error found (unvalid lable) returns 1 else 0
 * @disctiption: checks if the lable id valid 
 */
int check_lable(char * cur_word, int print_flag,char * file_name ,int line_num,Node * macro_list);
 
/**
 * @param command_name a string with the cuurent command_name
 * @param input - a pointer to the string that contains the line from file
 * @param codeword_arr - a pointer to the array that contains the lables 
 * @param IC - a pointer to the instructions count 
 * @param unknown_lables_arr - a array to put in the lables that the adress is not known saved the name and index of codeword
 * @param unknown_lbl_size - the size of unknown unknown_lable_arr
 * @discription: Function gets input and command name and updates the codeword arr with the needed value
 */
void update_code_word(char * command_name,char ** input,code_word * codeword_arr,int * IC,unknown_lables ** unknown_lables_arr, int * unknown_lbl_size,char*file_name,int line_num,Node * macro_list);

/** 
 * @param codeword_arr - a pointer to the array that contains the lables 
 * @param operand a string with the cuurent operand from command line
 * @param IC - a pointer to the instructions count
 * @param type_operand if operand is direct emidiet and register type
 * @param unknown_lables_arr - a array to put in the lables that the adress is not known saved the name and index of codeword
 * @discription: Function gets the codewordarr for operands. it updates the arr acording to rules of operand
 */

void update_operand_codeword(code_word * codeword_arr,char* operand ,int * IC,int type_operand,int op_num,unknown_lables ** unknown_lables_arr,int * unknown_lbl_count,int line_num);

/**
 * @param word - a word to check
 * @param macro_list - a list of macro to make sure are differnt then the lables
 * @return if word is reserved returns 1 else 0
 * @disctiption: checks if word that is sent is not reservd
 */
int reserved(char * lable_name ,Node * macro_list); 

/**
 * @param command - the word to check what command (if command)
 * @return if word is command returns the command opcode if not returns -1
 * @disctiption: checks if word is command and returns opcode if is 
 */
int get_opcode(char* command);

/**
 * @param cur_word - the command name
 * @return number of operands for command
 * @disctiption: checks and returns the number of operands for command
 */
int num_operands_command(char* cur_word);

/**
 * @param entries - a pointer to entries array(the array to update)
 * @param cur_word - a string that has the current lable
 * @disctiption: updates the entries string if a lable is intialized as enrty
 */
void update_entries(entry_l **entries, char *cur_word, int line_number,int * entries_count);

/**
 * @param operand - the operand to check (string)
 * @return if register returns register number(in binary) else -1
 * @disctiption: checks and returns number of register if not reg. returns -1
 */
int is_register(char* operand);

/**
 * @param operand - the operand to check (string)
 * @param operand_num - the number of the operand (sourse or target)
 * @return operands type in binary code if not valid input returns -1
 * @disctiption: gets operand and reterns operands type in binary code
 */
int type_operand(char * operand,int operand_num,char * file_name,int line_num,Node * macro_list);


/**
 * @discription: converts givin number to string 
 */
void int_to_str(int num, char *str);


#endif
