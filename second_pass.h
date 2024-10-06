#ifndef SECOND_PASS
#define SECOND_PASS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * @param entries - the array of all the lables intialized as entry in first pass
 * @param lable_arr - the array of intialized lables (not entry ,exter)
 * @param lable_count - the amount of lables in lable array
 * @param file_name - the current file name
 * @param entrie_count- the amount of entries in entries lable
 * @return returns 1 if error founr in funct if not returns 0
 * @discription - the function checks if all lables that are declared as entry are intialized in file
 */
int check_entries(entry_l * entries,lable * lable_arr, int lable_count,char * file_name, int entries_count);

/**
 * @param codword_arr - the array of codwords
 * @param unknown_lable_arr - an array for lables used in commands 
 * @param unknoun_lbl_count - the amount of lables in unknown_lables array
 * @param lable_arr - array of intialized lables 
 * @param lable_count - the amount of lables in lable array
 * @param file_name - the current file name
 * @discription - the function updates all the code words for the lables that are used in commands
 */
int second_pass_update(code_word * codeword_arr ,unknown_lables * unknown_lables_arr , int unknown_lbl_count,lable * lable_arr,int lable_count,char *file_name);

/**
 * @param file_name - the current files name
 * @param lable_arr - array of intialized lables 
 * @param lable_count - the amount of lables in lable array
 * @param IC -number of Instrucions
 * @param DC -number of Data 
 * @param entries_count - amount of lables declaired as entry
 * @param codword_arr - the array of codwords
 * @param data_arr - the array that contains the data
 * @param unknown_lable_arr - an array for lables used in commands 
 * @param unknoun_lbl_count - the amount of lables in unknown_lables array
 * @discription - the function creates entry , extern, ob file. only if needed
 */
void create_files(char* file_name ,lable *lable_arr, int lable_count, int IC, int DC, int entries_count, code_word *codeword_arr,int * data_arr,unknown_lables * unknown_lales_arr,int unknown_lbl_count );


/**
 * @param lable_arr - array of intialized lables 
 * @param IC - number of instructions
 * @param lable_count - the amount of lables in lable array
 * @discription - function updates the addres of the lables in lable_arr to: if befor command adress+100 if before data address+IC+100
 */
void address_update(lable lable_arr[], int IC, int lable_count);

/**
 * @discription - function frees lables arrays.
*/
void free_lables_array(lable * lable_arr,int lable_count, unknown_lables * unknown_lables_arr, int unknown_lbl_count,entry_l * entries,int entries_count);
#endif
